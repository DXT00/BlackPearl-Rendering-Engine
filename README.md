

1.添加了ObjectManager,和ObjectCreater 类

ObjectManager用于的管理Object列表 和 Draw Objects

ObjectCreater 采用工厂模式生成 Object


2.添加 MeshFilter 存储 Object的 数据（vertices 和 Indices）


3.通过ImGui 控制生成 Cube,Sphere(还未添加)这些基本3D Object

把Component参数放到了ImGui,使得参数可调。

光源可通过LightComponent改变颜色 大小。



1. ObjectManager and ObjectCreater classes have been added

ObjectManager is used to manage the list of Objects and Draw Objects

ObjectCreater USES the factory pattern to generate objects


2. Add MeshFilter to store the data of objects (vertices and Indices)


3. Generate Cube,Sphere(not yet added) these basic 3D objects through ImGui control.

Put Component parameters in the ImGui to make the parameters tunable.

Light sources can change color sizes through LightComponent.