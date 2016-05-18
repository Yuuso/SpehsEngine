
#include "PostProcessing.h"
#include "ShaderManager.h"
#include "OpenGLError.h"
#include "Exceptions.h"
#include "ApplicationData.h"

#include <GL/glew.h>


GLuint frameBufferObject = 0;
GLuint frameBufferVertexBufferObject = 0;
GLuint frameBufferObjectTextureID = 0;
GLuint renderBufferObject = 0;

bool postProcessingStarted = false;
bool usePostProcessing = false;
int shaderIndex = spehs::DefaultPostProc;


namespace spehs
{
	namespace graphics
	{
		namespace postproc
		{
			void postProcessingBegin()
			{
				if (usePostProcessing)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
					postProcessingStarted = true;
				}
			}
			void postProcessingEnd()
			{
				if (usePostProcessing && postProcessingStarted)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
					glClearColor(0.0, 0.0, 0.0, 1.0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					shaderManager->use(shaderIndex);
					shaderManager->getShader(shaderIndex)->uniforms->textureDataID = frameBufferObjectTextureID;
					shaderManager->setUniforms(shaderIndex);

					glBindBuffer(GL_ARRAY_BUFFER, frameBufferVertexBufferObject);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glDisableVertexAttribArray(0);

					checkOpenGLErrors(__FILE__, __LINE__);
				}
				postProcessingStarted = false;
			}

			void setPostProcessingShader(const int _shaderIndex)
			{
				shaderIndex = _shaderIndex;
			}
			void enablePostProcessing()
			{
				//Initialize
				//Texture
				glActiveTexture(GL_TEXTURE0);
				glGenTextures(1, &frameBufferObjectTextureID);
				glBindTexture(GL_TEXTURE_2D, frameBufferObjectTextureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, applicationData->getWindowWidth(), applicationData->getWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glBindTexture(GL_TEXTURE_2D, 0);

				//Render Buffer
				glGenRenderbuffers(1, &renderBufferObject);
				glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, applicationData->getWindowWidth(), applicationData->getWindowHeight());
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				//Frame Buffer
				glGenFramebuffers(1, &frameBufferObject);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferObjectTextureID, 0);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					exceptions::fatalError("glCheckFramebufferStatus: error " + std::to_string(status));
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				
				//Vertex Buffer
				const GLfloat frameBufferVertices [] =
				{
					-1.0f, -1.0f,
					1.0f, -1.0f,
					-1.0f, 1.0f,
					1.0f, 1.0f,
				};
				glGenBuffers(1, &frameBufferVertexBufferObject);
				glBindBuffer(GL_ARRAY_BUFFER, frameBufferVertexBufferObject);
				glBufferData(GL_ARRAY_BUFFER, sizeof(frameBufferVertices), frameBufferVertices, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				checkOpenGLErrors(__FILE__, __LINE__);

				usePostProcessing = true;
			}
			void disablePostProcessing()
			{
				if (usePostProcessing)
				{
					//Uninitialize
					glDeleteRenderbuffers(1, &renderBufferObject);
					glDeleteTextures(1, &frameBufferObjectTextureID);
					glDeleteFramebuffers(1, &frameBufferObject);
					glDeleteBuffers(1, &frameBufferVertexBufferObject);

					checkOpenGLErrors(__FILE__, __LINE__);

					usePostProcessing = false;
				}
			}
			bool postProcessingEnabled()
			{
				return usePostProcessing;
			}
			void postProcessingReshape()
			{
				if (usePostProcessing)
				{
					glBindTexture(GL_TEXTURE_2D, frameBufferObject);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, applicationData->getWindowWidth(), applicationData->getWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
					glBindTexture(GL_TEXTURE_2D, 0);

					glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, applicationData->getWindowWidth(), applicationData->getWindowHeight());
					glBindRenderbuffer(GL_RENDERBUFFER, 0);
				}
			}
		}
	}
}