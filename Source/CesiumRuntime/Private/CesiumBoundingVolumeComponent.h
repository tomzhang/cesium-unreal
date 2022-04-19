// Copyright 2020-2021 CesiumGS, Inc. and Contributors

#pragma once

#include "Components/PrimitiveComponent.h"	
#include "PrimitiveSceneProxy.h"
#include "CoreMinimal.h"
#include <glm/mat4x4.hpp>
#include <Cesium3DTilesSelection/BoundingVolume.h>
#include <Cesium3DTilesSelection/TileOcclusionRendererProxy.h>
#include <optional>
#include "CesiumBoundingVolumeComponent.generated.h"

class ACesiumGeoreference;

UCLASS()
class UCesiumBoundingVolumePoolComponent : 
public UActorComponent
public Cesium3DTilesSelection::TileOcclusionRendererProxyPool {
  GENERATED_BODY()

public:
  ACesiumGeoreference* Georeference;

protected:
  TileOcclusionRendererProxy* createProxy() override;

  void destroyProxy(TileOcclusionRendererProxy* pProxy) override;
};

UCLASS()
class UCesiumBoundingVolumeComponent : 
public UPrimitiveComponent, public Cesium3DTilesSelection::TileOcclusionRendererProxy {
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UCesiumBoundingVolumeComponent() = default;
  virtual ~UCesiumBoundingVolumeComponent() = default;

  FPrimitiveSceneProxy* CreateSceneProxy() override;

  /**
   * Set the occlusion result for this bounding volume, or nullopt if there 
   * was not a definitive occlusion result this frame.
   * 
   * @param isOccluded The occlusion result.
   */
  void SetOcclusionResult_RenderThread(const std::optional<bool>& isOccluded);
  
  /**
   * Notifies that the render thread occlusion retrieval task is complete and 
   * has not been re-queued up yet. Upon receiving this notification, the 
   * component will save the render thread result onto a game thread 
   * reflection. Called on the game thread. 
   */
  void SyncOcclusionResult();

  /**
   * Updates this component's transform from a new double-precision
   * transformation from the Cesium world to the Unreal Engine world, as well as
   * the current HighPrecisionNodeTransform.
   *
   * @param CesiumToUnrealTransform The new transformation.
   */
  void UpdateTransformFromCesium(const glm::dmat4& CesiumToUnrealTransform);

  //virtual void BeginDestroy() override;

// TileOcclusionRendererProxy implementation
  bool isOccluded() const override;

  int32_t getLastUpdatedFrame() const override;


  struct OcclusionRenderThreadResult {
    // 
    std::optional<bool> isOccluded;
  };

protected:
  void reset(const Cesium3DTilesSelection::Tile* pTile) override;

  void update(int32_t currentFrame) override;

private: 
  
  // Updated in render thread 
  std::optional<bool> _isOccluded_RenderThread;

  // Game thread safe information. Synced from the render thread result
  bool _isOccluded = false;
  bool _occlusionUpdatedThisFrame = false;
  int32_t _lastUpdatedFrame = -1000;
  
  // TODO:
  // Do these need to be accessed on the game thread, render thread?
  // Do they get reloaded on render state reload? 
  Cesium3DTilesSelection::BoundingVolume _tileBounds;
  glm::dmat4 _tileTransform;
};
