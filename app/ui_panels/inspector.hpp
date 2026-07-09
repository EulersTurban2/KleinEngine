#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP

#include "all.hpp"
#include <imgui.h>

#include <algorithm>
#include <cstring>

namespace App {

    class InspectorPanel {
        public:
            InspectorPanel() = default;

            static void onImGuiRender(HyperbolicProjection& currProj,
                                      Scene& mainScene, uint32_t previewTexID,
                                      glm::vec3& objColor,
                                      glm::vec3& pos,
                                      glm::vec3& rot,
                                      glm::vec3& scale,
                                      std::shared_ptr<Model>& previewModel,
                                      std::shared_ptr<Material>& previewMat) {

                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImVec2 windowSize(750, 500);
                ImVec2 windowPos(
                    viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
                    viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f
                );

                ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

                ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

                ImGui::Begin("Klein Engine Settings", nullptr, windowFlags);

                drawProjectionSetting(currProj);
                drawCameraControls(mainScene);
                drawEntityInspector(mainScene);
                drawEntityCreator(mainScene, previewModel, previewMat, pos, rot, scale);
                drawModelPreviewer(previewTexID, objColor, rot, pos, scale);
                ImGui::End();
            }

        private:
            static void drawProjectionSetting(HyperbolicProjection& currentProjection) {
                ImGui::Text("Projection Model");

                int projectionIdx = static_cast<int>(currentProjection);
                bool changed = false;

                changed |= ImGui::RadioButton("Klein", &projectionIdx, 0);
                ImGui::SameLine();
                changed |= ImGui::RadioButton("Poincare", &projectionIdx, 1);
                ImGui::SameLine();
                changed |= ImGui::RadioButton("Half-space", &projectionIdx, 2);

                if (changed) {
                    currentProjection = static_cast<HyperbolicProjection>(projectionIdx);
                }

                // Half-space uses its own orbit camera; expose it for live tuning.
                if (currentProjection == HyperbolicProjection::HalfSpace) {
                    ImGui::Spacing();
                    ImGui::Text("Half-space Camera");
                    auto& v = Renderer::halfSpaceView();
                    ImGui::SliderFloat("Distance",      &v.distance,     0.5f,  15.0f);
                    ImGui::SliderFloat("Azimuth",       &v.azimuth,   -180.0f, 180.0f);
                    ImGui::SliderFloat("Elevation",     &v.elevation,   -5.0f,  89.0f);
                    ImGui::SliderFloat("Target Height", &v.targetHeight, 0.0f,   3.0f);
                    ImGui::SliderFloat("FOV",           &v.fov,         20.0f, 100.0f);
                }

                ImGui::Separator();
            }

            static void drawCameraControls(Scene& scene) {
                Camera& cam = scene.getCamera();
                ImGui::Text("Camera");

                float speed = cam.getMovementSpeed();
                if (ImGui::SliderFloat("Movement Speed", &speed, 0.1f, 15.0f)) {
                    cam.setMovementSpeed(speed);
                }

                float sensitivity = cam.getMouseSensitivity();
                if (ImGui::SliderFloat("Look Sensitivity", &sensitivity, 0.01f, 1.0f)) {
                    cam.setMouseSensitivity(sensitivity);
                }

                ImGui::Separator();
            }

            static void drawEntityInspector(Scene& scene) {
                ImGui::Text("Active Entities");

                auto& registry = scene.getRegistry();

                std::vector<uint32_t> activeEntities = registry.view<TagComponent>();

                static uint32_t selectedEntityId = 0; // 0 represents "No Entity Selected"

                // 1. Safety check: If the selected entity was destroyed, reset selection
                if (selectedEntityId != 0 && !registry.hasComponent<TagComponent>(selectedEntityId)) {
                    selectedEntityId = 0;
                }

                // 2. Setup the Combobox Preview
                std::string previewValue = "Select an Entity";
                if (selectedEntityId != 0) {
                    auto& tag = registry.getComponent<TagComponent>(selectedEntityId);
                    previewValue = tag.name + " (ID: " + std::to_string(selectedEntityId) + ")";
                }

                // 3. Render the Combobox
                if (ImGui::BeginCombo("Scene Entities", previewValue.c_str())) {
                    for (uint32_t id : activeEntities) {
                        auto& tag = registry.getComponent<TagComponent>(id);
                        std::string displayName = tag.name + " (ID: " + std::to_string(id) + ")";

                        const bool isSelected = (selectedEntityId == id);
                        if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                            selectedEntityId = id;
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::Separator();

                // 4. Render Characteristics (Only if an entity is selected and has a transform)
                if (selectedEntityId != 0 && registry.hasComponent<TransformComponent>(selectedEntityId)) {
                    auto& transform = registry.getComponent<TransformComponent>(selectedEntityId);

                    ImGui::Text("Transform Characteristics");

                    bool transformChanged = false;

                    if (ImGui::DragFloat3("Position", &transform.position.x, 0.1f)) transformChanged = true;

                    // Rotation in degrees for UX
                    if (ImGui::SliderFloat3("Rotation", &transform.rotation.x, -180.0f, 180.0f)) transformChanged = true;

                    if (ImGui::DragFloat3("Scale", &transform.scale.x, 0.05f, 0.001f, 100.0f)) transformChanged = true;

                    // Dragging a slider moves the entity, so the spatial index must follow
                    if (transformChanged && scene.getSpatialIndex() != nullptr) {
                        scene.getSpatialIndex()->update(selectedEntityId, transform.position);
                    }
                }
            }

            static void drawEntityCreator(Scene& scene, std::shared_ptr<Model>& previewModel, std::shared_ptr<Material>& previewMat,
                                          const glm::vec3& spawnPos, const glm::vec3& spawnRot, const glm::vec3& spawnScale) {
                ImGui::Text("Create New Entity");

                auto names = entityLoader.getAllEntityNames();
                std::vector<const char*> c_names;
                c_names.reserve(names.size());
                for (const auto& name : names) {
                    c_names.push_back(name.c_str());
                }

                static int selectedPrefabIdx = 0;
                static char instanceName[128] = "New Entity";
                static bool isFirstRun = true;

                if (isFirstRun && !names.empty()) {
                    auto modelProperties = entityLoader.getEntityMetadata(names[0]);
                    auto baseMat = entityLoader.getMaterial(modelProperties["material"]);
                    previewMat = std::make_shared<Material>(*baseMat);
                    previewModel = entityLoader.getModel(modelProperties["model"]);
                    isFirstRun = false;
                }

                ImGui::InputText("Instance Name", instanceName, IM_ARRAYSIZE(instanceName));
                if (ImGui::Combo("Prefab", &selectedPrefabIdx, c_names.data(), static_cast<int>(c_names.size()))) {
                    auto modelProperties = entityLoader.getEntityMetadata(names[selectedPrefabIdx]);
                    auto baseMat = entityLoader.getMaterial(modelProperties["material"]);
                    previewMat = std::make_shared<Material>(*baseMat);
                    previewModel = entityLoader.getModel(modelProperties["model"]);
                }
                ImGui::Spacing();

                if (ImGui::Button("Spawn into Scene", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                    if (!names.empty() && selectedPrefabIdx >= 0 && selectedPrefabIdx < static_cast<int>(names.size())) {
                        Entity newEnt = entityLoader.instantiate(names[selectedPrefabIdx], scene);

                        newEnt.getComponent<TagComponent>().name = std::string(instanceName);
                        auto& transform = newEnt.getComponent<TransformComponent>();
                        transform.position = spawnPos;
                        transform.rotation = spawnRot;
                        transform.scale = spawnScale;

                        if (scene.getSpatialIndex() != nullptr) {
                            scene.getSpatialIndex()->insert(newEnt.getId(), transform.position);
                        }

                        std::strcpy(instanceName, "New Entity");
                    }
                }

                ImGui::Separator();
            }

            static void drawModelPreviewer(uint32_t previewTextureID, glm::vec3& objectColor, glm::vec3& modelRotation,
                                           glm::vec3& position, glm::vec3& scale) {

                ImGui::Separator();
                ImGui::Text("Preview & Configuration");
                ImGui::Spacing();

                if (ImGui::BeginTable("PreviewLayoutTable", 2)) {

                    // --- LEFT COLUMN: The Live Preview Image ---
                    ImGui::TableNextColumn();
                    ImGui::Text("Live Preview (Click & Drag to Rotate)");

                    ImVec2 contentSize = ImGui::GetContentRegionAvail();
                    // Keep it perfectly square, but constrain it to the column width
                    float size = std::min(contentSize.x, 250.0f);

                    if (previewTextureID != 0) {
                        ImGui::Image(
                            (ImTextureID)(intptr_t)previewTextureID,
                            ImVec2(size, size),
                            ImVec2(0.0f, 1.0f),
                            ImVec2(1.0f, 0.0f)
                        );

                        if (ImGui::IsItemHovered()) {
                            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                                ImVec2 delta = ImGui::GetIO().MouseDelta;
                                // Rotation is stored in degrees
                                modelRotation.y += delta.x * 0.5f;
                                modelRotation.x += delta.y * 0.5f;
                            }
                        }
                    } else {
                        // Placeholder box if no texture
                        ImGui::Dummy(ImVec2(size, size));
                        ImGui::Text("No preview available");
                    }

                    ImGui::TableNextColumn();

                    ImGui::Text("Material Settings");
                    ImGui::PushItemWidth(-1.0f);
                    ImGui::ColorEdit3("##Albedo Tint", &objectColor.x);

                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Text("Transform Constraints");

                    ImGui::Text("Position");
                    ImGui::DragFloat3("##Pos", &position.x, 0.1f);

                    ImGui::Text("Rotation");
                    ImGui::DragFloat3("##Rot", &modelRotation.x, 1.0f);

                    ImGui::Text("Scale");
                    ImGui::DragFloat3("##Scale", &scale.x, 0.05f, 0.001f, 100.0f);

                    ImGui::PopItemWidth();

                    // End the table layout
                    ImGui::EndTable();
                }
            }
    };
}

#endif // INSPECTOR_HPP
