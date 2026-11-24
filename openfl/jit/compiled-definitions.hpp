#pragma once
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "definition-layouts.hpp"

                                                                              
                                                                                 
namespace OpenPVZDefinitions {

inline uint32_t legacyHash(DefMap* map, uint32_t hash, std::vector<DefMap*>& seen)
{
    for (auto* previous : seen) if (previous == map) return hash;
    seen.push_back(map);
    auto layout = legacyLayout(map);
    hash = crc32(hash, reinterpret_cast<const Bytef*>(&layout.data[0]), 4);
    size_t index = 0;
    for (auto* field = map->mMapFields; *field->mFieldName; ++field, ++index)
    {
        if (index >= layout.fields) throw std::runtime_error("Compiled definition field count changed");
        hash = crc32(hash, reinterpret_cast<const Bytef*>(&field->mFieldType), 4);
        hash = crc32(hash, reinterpret_cast<const Bytef*>(&layout.data[index + 1]), 4);
        if (field->mFieldType == DefFieldType::DT_ARRAY)
            hash = legacyHash(static_cast<DefMap*>(field->mExtraData), hash, seen);
        else if (field->mFieldType == DefFieldType::DT_ENUM || field->mFieldType == DefFieldType::DT_FLAGS)
            hash = DefinitionCalcHashSymbolMap(hash, static_cast<DefSymbol*>(field->mExtraData));
    }
    if (index != layout.fields) throw std::runtime_error("Compiled definition field count changed");
    return hash;
}

class Reader
{
    const unsigned char* cursor;
    size_t remaining;
    bool legacy;
    size_t allocated = 0;
    std::vector<std::unique_ptr<char[]>> storage;
    static constexpr size_t Limit = 256 * 1024 * 1024;

    const unsigned char* take(size_t count)
    {
        if (count > remaining) throw std::runtime_error("Truncated compiled definition");
        const auto* result = cursor;
        cursor += count;
        remaining -= count;
        return result;
    }
    int32_t integer()
    {
        int32_t value;
        std::memcpy(&value, take(4), 4);
        return value;
    }
    static size_t product(int count, size_t stride)
    {
        if (count < 0 || stride == 0 || static_cast<size_t>(count) > Limit / stride)
            throw std::runtime_error("Invalid compiled definition count");
        return static_cast<size_t>(count) * stride;
    }
    char* allocate(size_t size)
    {
        if (size > Limit - allocated) throw std::runtime_error("Compiled definition allocation limit exceeded");
        auto data = std::make_unique<char[]>(size);
        auto* result = data.get();
        storage.push_back(std::move(data));
        allocated += size;
        return result;
    }
    size_t wireSize(DefMap* map) const
    {
        return legacy ? static_cast<size_t>(legacyLayout(map).data[0]) : static_cast<size_t>(map->mDefSize);
    }
    void scalarBody(DefMap* map, void* destination, const unsigned char* body)
    {
        std::memset(destination, 0, map->mDefSize);
        if (map->mConstructorFunc) map->mConstructorFunc(destination);
        auto layout = legacyLayout(map);
        size_t index = 0;
        for (auto* field = map->mMapFields; *field->mFieldName; ++field, ++index)
        {
            if (index >= layout.fields) throw std::runtime_error("Compiled definition schema changed");
            const size_t offset = legacy ? layout.data[index + 1] : field->mFieldOffset;
            auto* output = static_cast<unsigned char*>(destination) + field->mFieldOffset;
            size_t span = 4;
            if (field->mFieldType == DefFieldType::DT_VECTOR2) span = 8;
            else if (field->mFieldType == DefFieldType::DT_ARRAY || field->mFieldType == DefFieldType::DT_TRACK_FLOAT)
                span = (legacy ? 4 : sizeof(void*)) + 4;
            else if (field->mFieldType == DefFieldType::DT_STRING || field->mFieldType == DefFieldType::DT_IMAGE || field->mFieldType == DefFieldType::DT_FONT)
                span = legacy ? 4 : sizeof(void*);
            if (offset > wireSize(map) || span > wireSize(map) - offset)
                throw std::runtime_error("Invalid compiled definition field offset");
            switch (field->mFieldType)
            {
            case DefFieldType::DT_INT:
            case DefFieldType::DT_FLOAT:
            case DefFieldType::DT_ENUM:
            case DefFieldType::DT_FLAGS:
            case DefFieldType::DT_VECTOR2:
                std::memcpy(output, body + offset, span);
                break;
            case DefFieldType::DT_ARRAY:
                std::memcpy(&reinterpret_cast<DefinitionArrayDef*>(output)->mArrayCount,
                    body + offset + (legacy ? 4 : sizeof(void*)), 4);
                break;
            case DefFieldType::DT_STRING:
            case DefFieldType::DT_IMAGE:
            case DefFieldType::DT_FONT:
            case DefFieldType::DT_TRACK_FLOAT:
                break;                                                                  
            default:
                throw std::runtime_error("Unknown compiled definition field type");
            }
        }
    }
    std::string text()
    {
        const int32_t size = integer();
        if (size < 0 || size > 1024 * 1024) throw std::runtime_error("Invalid compiled definition string length");
        const auto* data = take(static_cast<size_t>(size));
        return std::string(reinterpret_cast<const char*>(data), size);
    }
    void fixups(DefMap* map, void* destination, int depth)
    {
        if (depth > 32) throw std::runtime_error("Compiled definition nesting limit exceeded");
        for (auto* field = map->mMapFields; *field->mFieldName; ++field)
        {
            auto* output = static_cast<unsigned char*>(destination) + field->mFieldOffset;
            switch (field->mFieldType)
            {
            case DefFieldType::DT_STRING:
            {
                auto value = text();
                const char* pointer = "";
                if (!value.empty())
                {
                    auto* string = allocate(value.size() + 1);
                    std::memcpy(string, value.data(), value.size());
                    string[value.size()] = '\0';
                    pointer = string;
                }
                std::memcpy(output, &pointer, sizeof(pointer));
                break;
            }
            case DefFieldType::DT_IMAGE:
            {
                auto value = text();
                if (!DefinitionLoadImage(reinterpret_cast<Image**>(output), value))
                    throw std::runtime_error("Missing compiled-definition image: " + value);
                break;
            }
            case DefFieldType::DT_FONT:
            {
                auto value = text();
                if (value.empty()) break;
                if (!DefinitionLoadFont(reinterpret_cast<Font**>(output), value))
                    throw std::runtime_error("Missing compiled-definition font: " + value);
                break;
            }
            case DefFieldType::DT_TRACK_FLOAT:
            {
                static_assert(sizeof(FloatParameterTrackNode) == 20, "Float track wire format changed");
                auto* track = reinterpret_cast<FloatParameterTrack*>(output);
                const int count = integer();
                const size_t bytes = product(count, sizeof(FloatParameterTrackNode));
                const auto* nodes = take(bytes);
                track->mCountNodes = count;
                if (count)
                {
                    track->mNodes = reinterpret_cast<FloatParameterTrackNode*>(allocate(bytes));
                    std::memcpy(track->mNodes, nodes, bytes);
                }
                break;
            }
            case DefFieldType::DT_ARRAY:
            {
                auto* array = reinterpret_cast<DefinitionArrayDef*>(output);
                auto* child = static_cast<DefMap*>(field->mExtraData);
                const size_t stride = wireSize(child);
                if (integer() != static_cast<int32_t>(stride))
                    throw std::runtime_error("Compiled definition array layout mismatch");
                const int count = array->mArrayCount;
                const auto* bodies = take(product(count, stride));
                if (!count) break;
                auto* data = allocate(product(count, child->mDefSize));
                array->mArrayData = data;
                for (int i = 0; i < count; ++i)
                    scalarBody(child, data + static_cast<size_t>(i) * child->mDefSize,
                        bodies + static_cast<size_t>(i) * stride);
                for (int i = 0; i < count; ++i)
                    fixups(child, data + static_cast<size_t>(i) * child->mDefSize, depth + 1);
                break;
            }
            default: break;
            }
        }
    }

public:
    Reader(const unsigned char* bytes, size_t size, bool legacyFormat)
        : cursor(bytes), remaining(size), legacy(legacyFormat) {}
    void read(DefMap* map, void* destination)
    {
        scalarBody(map, destination, take(wireSize(map)));
        fixups(map, destination, 0);
        if (remaining) throw std::runtime_error("Extra bytes in compiled definition");
        for (auto& allocation : storage) allocation.release();
    }
};

inline bool read(const unsigned char* data, size_t size, DefMap* map, void* destination, std::string& error)
{
    try
    {
        if (size < 4) throw std::runtime_error("Compiled definition has no schema hash");
        uint32_t hash;
        std::memcpy(&hash, data, 4);
        std::vector<DefMap*> seen;
        const bool legacy = hash == legacyHash(map, 1, seen);
        if (!legacy && hash != DefinitionCalcHash(map))
            throw std::runtime_error("Unsupported compiled definition schema hash: " + std::to_string(hash));
        Reader reader(data + 4, size - 4, legacy);
        reader.read(map, destination);
        error.clear();
        return true;
    }
    catch (const std::exception& exception)
    {
        std::memset(destination, 0, map->mDefSize);
        if (map->mConstructorFunc) map->mConstructorFunc(destination);
        error = exception.what();
        return false;
    }
}

}                                
