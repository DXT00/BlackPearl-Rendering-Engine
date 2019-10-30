通过ImGui界面创建 Object


ImGui 的回调处理 全部通过 Layer class处理

包括 Create各种 Objects, DrawObjects,DestroyObjects等事件.....


一个Appliacation可以有多个Scenes,但当前运行的只有一个Scene!

一个Scene 可以有多个 Layer,每个Layer都有自己的EntityManager

渲染是以Layer为单位渲染的！

Application运行 ---> Scene Update --->Sence中的每个Layer Update(Draw())

		