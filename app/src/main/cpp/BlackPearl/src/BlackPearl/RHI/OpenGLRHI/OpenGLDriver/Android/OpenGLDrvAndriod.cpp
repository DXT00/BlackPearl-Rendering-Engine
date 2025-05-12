#include "pch.h"

#include "Android/OpenGLDrvAndroid.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"

namespace  BlackPearl{
    void PlatformBindContextVAO(FPlatformOpenGLDevice* Device) {
//        HGLRC Context = GetCurrentContext();
//
//        if (Context == Device->RenderingContext.OpenGLContext)	// most common case
//        {
//            //assert(glIsVertexArray(Device->RenderingContext.VertexArrayObject));
//
//            GLint currentVAO;
//            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
//
//            if (currentVAO == static_cast<GLint>(Device->RenderingContext.VertexArrayObject)) {
//                GE_CORE_INFO("VAO %d", Device->RenderingContext.VertexArrayObject);
//                //std::cout << "VAO " << Device->RenderingContext.VertexArrayObject << " �Ѱ�����Ч��" << std::endl;
//            }
//            else {
//                GE_CORE_WARN("VAO invalid %d", Device->RenderingContext.VertexArrayObject);
//                //std::cerr << "VAO " << Device->RenderingContext.VertexArrayObject << " δ�󶨻���Ч��" << std::endl;
//            }
//            glBindVertexArray(Device->RenderingContext.VertexArrayObject);
//
//
//        }
//        else if (Context == Device->SharedContext.OpenGLContext)
//        {
//            glBindVertexArray(Device->SharedContext.VertexArrayObject);
//
//        }


        GLuint *DefaultVao = nullptr;
        EOpenGLCurrentContext ContextType = (EOpenGLCurrentContext) AndroidEGL::GetInstance()->GetCurrentContextType();

        if (ContextType == CONTEXT_Rendering) {
            DefaultVao = &AndroidEGL::GetInstance()->GetRenderingContext()->DefaultVertexArrayObject;
        } else if (ContextType == CONTEXT_Shared) {
            DefaultVao = &AndroidEGL::GetInstance()->GetSharedContext()->DefaultVertexArrayObject;
        } else {
            //Invalid or Other return
            return;
        }

        if (*DefaultVao == 0) {
            glGenVertexArrays(1, DefaultVao);
            glBindVertexArray(*DefaultVao);
        }else{
            glBindVertexArray(*DefaultVao);

        }


    }
}
