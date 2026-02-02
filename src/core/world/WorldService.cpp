#include <core/world/WorldService.h>
#include <core/base/Actor.h>
#include <core/rendering/Camera.h>
#include <core/rendering/RenderContextService.h>

WorldService::WorldService()
    : Services(GetHost())
    , UI(std::make_unique<UIManager>())
{
}

WorldService::~WorldService() = default;

void WorldService::SetBackgroundTexture(TextureHandle texture)
{
    BackgroundTexture = texture;
}

void WorldService::SetCameraTarget(Actor* target)
{
    CameraTarget = target;
}

void WorldService::UpdateCameraTarget()
{
    if (CameraTarget && CameraTarget->IsEnabled()) {
        // Only update the primary context's camera (game camera)
        // Debug contexts have independent cameras
        auto* contextService = Services.TryGet<RenderContextService>();
        if (contextService) {
            RenderContext* primaryContext = contextService->GetPrimaryContext();
            if (primaryContext) {
                primaryContext->ContextCamera.SetTarget(CameraTarget->GetPosition());
            }
        }
    }
}
