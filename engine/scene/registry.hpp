#ifndef __REGISTRY_HPP
#define __REGISTRY_HPP

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>


namespace Engine::Scene {

    class IComponentArray {
        public:
            virtual ~IComponentArray() = default;
            virtual void entityDestroyed(uint32_t entity) = 0;
    };


    template<typename T>
    class ComponentArray : public IComponentArray {
        public:
            void insertData(uint32_t entity, T component) {
                mComponentData[entity] = component;
            }

            void removeData(uint32_t entity) {
                mComponentData.erase(entity);
            }

            T& getData(uint32_t entity) {
                return mComponentData[entity];
            }

            bool hasEntity(uint32_t entity) const {
                return mComponentData.find(entity) != mComponentData.end();
            }

            void entityDestroyed(uint32_t entity) override {
                removeData(entity);
            }
        private:
            std::unordered_map<uint32_t, T> mComponentData;
    };

    class Registry {
        public:

            Registry() : mNextEntityId(1) {}

            uint32_t createEntity() {return mNextEntityId++;}

            template<typename T>
            void addComponent(uint32_t entity, T component){
                getComponentArray<T>()->insertData(entity, component);
            }

            template<typename T>
            T& getComponent(uint32_t entity){
                return getComponentArray<T>()->getData(entity);
            }

            template<typename T>
            bool hasComponent(uint32_t entity){
                return getComponentArray<T>()->hasEntity(entity);
            }

            template<typename T>
            std::vector<uint32_t> view(){
                std::vector<uint32_t> entities;
                auto array = getComponentArray<T>();
                for (uint32_t i = 0; i < mNextEntityId; ++i)
                {
                    if (array->hasEntity(i))
                    {
                        entities.push_back(i);
                    }
                }
                return entities;
            }

        private:
            uint32_t mNextEntityId;
            std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> mComponentArrays;
    
            template<typename T>
            std::shared_ptr<ComponentArray<T>> getComponentArray(){
                std::type_index typeName = std::type_index(typeid(T));
                if (mComponentArrays.find(typeName) == mComponentArrays.end())
                {
                    mComponentArrays[typeName] = std::make_shared<ComponentArray<T>>();
                }
                return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
            }

    };

}

#endif