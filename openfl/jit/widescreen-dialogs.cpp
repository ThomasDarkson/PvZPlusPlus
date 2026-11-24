                                                                     
namespace OpenPVZDialogs
{
    std::map<std::string, Image*> images;
}
Image* pvz_host_dialog_image(const char* name)
{
    auto found = OpenPVZDialogs::images.find(name);
    if (found != OpenPVZDialogs::images.end()) return found->second;
    Image* image = gSexyAppBase->GetImage(std::string("../widescreen-assets/dialogs/") + name, false);
    if (!image) throw std::runtime_error(std::string("Missing widescreen dialog asset: ") + name);
    OpenPVZDialogs::images[name] = image;
    return image;
}
void pvz_host_free_dialog_images()
{
    for (auto& image : OpenPVZDialogs::images) delete image.second;
    OpenPVZDialogs::images.clear();
}
void pvz_host_draw_background(Graphics* g, const char* name)
{
    Graphics background(*g);
    background.ClearClipRect();
    background.SetColorizeImages(false);
    background.SetColor(Color::White);
    background.SetDrawMode(Graphics::DRAWMODE_NORMAL);
    background.DrawImage(pvz_host_dialog_image(name), -133, 0);
}

void pvz_host_fill_viewport(Graphics* g)
{
    Graphics fill(*g);
    fill.ClearClipRect();
    fill.mTransX = fill.mTransY = 0;
                                                                                
                                                                                
    fill.FillRect(0, 0, fill.mDestImage->GetWidth(), fill.mDestImage->GetHeight());
}
