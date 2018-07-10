#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Rendering/PostProcessing.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Input/Input.h"

#include <GL/glew.h>


GLuint frameBufferObjectMS = 0;
GLuint frameBufferObjectTextureIDMS = 0;
GLuint renderBufferObjectMS = 0;
GLuint colorBufferObjectMS = 0;

GLuint frameBufferObject = 0;
GLuint frameBufferObjectTextureID = 0;
GLuint renderBufferObject = 0;
GLuint colorBufferObject = 0;

GLuint frameBufferVertexBufferObject = 0;

bool postProcessingStarted = false;
bool usePostProcessing = false;
int shaderIndex = (int)spehs::ShaderName::DefaultPostProc;


namespace spehs
{
	namespace graphics
	{
		namespace postproc
		{
			void postProcessingBegin()
			{
				//TODO...
				//if (usePostProcessing)
				//{
				//	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectMS);
				//	glEnable(GL_MULTISAMPLE);
				//	postProcessingStarted = true;
				//}
			}
			void postProcessingEnd()
			{
				//TODO...
				//if (usePostProcessing && postProcessingStarted)
				//{
				//	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferObjectMS);
				//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObject);
				//	glBlitFramebuffer(0, 0, spehs::input::windowWidth, spehs::input::windowHeight, 0, 0, spehs::input::windowWidth, spehs::input::windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

				//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

				//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//	glDisable(GL_BLEND);
				//	glDepthMask(GL_TRUE);
				//	glEnable(GL_DEPTH_TEST);
				//	glCullFace(GL_BACK);
				//	glEnable(GL_CULL_FACE);

				//	shaderManager->use(shaderIndex);
				//	shaderManager->getShader(shaderIndex)->uniforms->textureDataID = frameBufferObjectTextureID;
				//	shaderManager->setUniforms(shaderIndex);

				//	glBindBuffer(GL_ARRAY_BUFFER, frameBufferVertexBufferObject);
				//	glEnableVertexAttribArray(0);
				//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
				//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				//	glDisableVertexAttribArray(0);
				//	glBindBuffer(GL_ARRAY_BUFFER, 0);
				//	shaderManager->unuse(shaderIndex);

				//	checkOpenGLErrors(__FILE__, __LINE__);
				//}
				//postProcessingStarted = false;
			}

			void setPostProcessingShader(const int _shaderIndex)
			{
				shaderIndex = _shaderIndex;
			}
			void enablePostProcessing()
			{
				////MultiSampling
				//{
				//	//Texture
				//	glActiveTexture(GL_TEXTURE0);
				//	glGenTextures(1, &frameBufferObjectTextureIDMS);
				//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferObjectTextureIDMS);

				//	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, spehs::input::windowWidth, spehs::input::windowHeight, GL_TRUE);

				//	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

				//	//Frame Buffer
				//	glGenFramebuffers(1, &frameBufferObjectMS);
				//	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectMS);

				//	//Color Buffer
				//	glGenRenderbuffers(1, &colorBufferObjectMS);
				//	glBindRenderbuffer(GL_RENDERBUFFER, colorBufferObjectMS);
				//	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA, spehs::input::windowWidth, spehs::input::windowHeight);
				//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBufferObjectMS);
				//	glBindRenderbuffer(GL_RENDERBUFFER, 0);

				//	//Depth Render Buffer
				//	glGenRenderbuffers(1, &renderBufferObjectMS);
				//	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObjectMS);
				//	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, spehs::input::windowWidth, spehs::input::windowHeight);
				//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObjectMS);
				//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObjectMS);

				//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferObjectTextureIDMS, 0);

				//	checkOpenGLErrors(__FILE__, __LINE__);

				//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				//	if (status != GL_FRAMEBUFFER_COMPLETE)
				//	{
				//		exceptions::fatalError("glCheckFramebufferStatus: error " + std::to_string(status));
				//	}
				//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//}
				//
				////Texture
				//glActiveTexture(GL_TEXTURE0);
				//glGenTextures(1, &frameBufferObjectTextureID);
				//glBindTexture(GL_TEXTURE_2D, frameBufferObjectTextureID);

				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spehs::input::windowWidth, spehs::input::windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

				//glBindTexture(GL_TEXTURE_2D, 0);

				////Frame Buffer
				//glGenFramebuffers(1, &frameBufferObject);
				//glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

				////Color Buffer
				//glGenRenderbuffers(1, &colorBufferObject);
				//glBindRenderbuffer(GL_RENDERBUFFER, colorBufferObject);
				//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, spehs::input::windowWidth, spehs::input::windowHeight);
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBufferObject);

				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferObjectTextureID, 0);

				//checkOpenGLErrors(__FILE__, __LINE__);

				//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				//if (status != GL_FRAMEBUFFER_COMPLETE)
				//{
				//	exceptions::fatalError("glCheckFramebufferStatus: error " + std::to_string(status));
				//}
				//glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//
				////Vertex Buffer
				//const GLfloat frameBufferVertices [] =
				//{
				//	-1.0f, -1.0f,
				//	1.0f, -1.0f,
				//	-1.0f, 1.0f,
				//	1.0f, 1.0f,
				//};
				//glGenBuffers(1, &frameBufferVertexBufferObject);
				//glBindBuffer(GL_ARRAY_BUFFER, frameBufferVertexBufferObject);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferVertices), frameBufferVertices, GL_STATIC_DRAW);
				//glBindBuffer(GL_ARRAY_BUFFER, 0);

				//checkOpenGLErrors(__FILE__, __LINE__);

				//usePostProcessing = true;
			}
			void disablePostProcessing()
			{
				//if (usePostProcessing)
				//{
				//	//Uninitialize
				//	glDeleteTextures(1, &frameBufferObjectTextureIDMS);
				//	glDeleteFramebuffers(1, &frameBufferObjectMS);
				//	glDeleteRenderbuffers(1, &renderBufferObjectMS);
				//	glDeleteRenderbuffers(1, &colorBufferObjectMS);

				//	glDeleteTextures(1, &frameBufferObjectTextureID);
				//	glDeleteFramebuffers(1, &frameBufferObject);
				//	glDeleteRenderbuffers(1, &colorBufferObject);

				//	glDeleteBuffers(1, &frameBufferVertexBufferObject);

				//	checkOpenGLErrors(__FILE__, __LINE__);

				//	usePostProcessing = false;
				//}
			}
			bool postProcessingEnabled()
			{
				return usePostProcessing;
			}
			void postProcessingReshape()
			{
				////TODO: fix
				//if (usePostProcessing)
				//{
				//	glBindTexture(GL_TEXTURE_2D, frameBufferObject);
				//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spehs::input::windowWidth, spehs::input::windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				//	glBindTexture(GL_TEXTURE_2D, 0);

				//	glBindRenderbuffer(GL_RENDERBUFFER, colorBufferObject);
				//	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, spehs::input::windowWidth, spehs::input::windowHeight);
				//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
				//}
			}
		}
	}
}