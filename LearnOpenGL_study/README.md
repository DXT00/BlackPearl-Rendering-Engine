添加 Entity和 Component,类似ECS的设计，不过没有分出system,逻辑处理部分还没设计


Camera 和 Light 都是Component


Object(Entity)的所有属性都由Coponent组成


EntityManager --->只管分配和回收 Entity


Object类通过继承Entity --->管理Component,每个Object都有多个Component


最后析构的时候还有一些问题，留着下次处理

现在只有CameraComponent和LightComponet ..模型渲染还没处理