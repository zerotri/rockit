#include <iostream>
#include <rockit/core/application.h>
#include <rockit/core/coroutine.h>
#include <rockit/core/pointer.h>
#include <rockit/core/array.h>
#include <rockit/platform/platform.h>
#include <rockit/core/actor.h>

using namespace Rockit;

auto vertShaderSource = u8R"---(
    #version 140
    in vec2 LVertexPos2D;
    void main()
    {
        gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 );
    }
)---";

auto fragShaderSource = u8R"---(
    #version 140
    out vec4 LFragment;
    void main()
    {
        LFragment = vec4( 1.0, 1.0, 1.0, 1.0 );
    }
)---";

int main(int argc, char **argv)
{
    MutableArray<std::string> stringArray(10);
    stringArray.Add("This is a string from main");

    Platform::LogInfo("Main", "Called from main function");
    Platform::LogInfo("Main", "%s", stringArray[0].c_str());

    class TestClass
    {
    public:
        TestClass()
        {
            Platform::LogInfo("TestClass", "TestClass ctor");
        }

        ~TestClass()
        {
            Platform::LogInfo("TestClass", "TestClass dtor");
        }
    };

    Coroutine coroutine(Coroutine::StackSize::Large,
        [stringArray](Coroutine &co, Coroutine::UserData userData) -> void *
        {
            TestClass testObj;
            Platform::LogInfo("Coroutine", "Called from within coroutine");
            Platform::LogInfo("Coroutine", "%s", stringArray[0].c_str());

            int i = 0;
            while (++i < 10)
            {
                Platform::LogInfo("Coroutine", ".");

                Yield(nullptr);
                // co.Wait(0.5, nullptr);
            }
            return nullptr;
        });

    double timer = 0.0;
    Actor testActor;

    int count = 0;

    testActor.AddBehaviour({
        .onUpdate = [&coroutine, &timer, &count](double deltaTime){
            timer += deltaTime;

            if(timer > 1)
            {
                Platform::LogInfo("Application", "timer: %fs", timer);

                if (coroutine.IsRunning())
                {
                    coroutine.Resume(nullptr);
                    fflush(nullptr);

                    if(++count > 3) {
                        coroutine.Cancel();
                    }
                }
                timer -= 1;
            }
        }
    });

    Application application({
        .name = "Rockit Demo",
        .width = 1280,
        .height = 720,
        .onLoad = [] {
            auto instance = Application::GetApplicationInstance();
            auto renderer = instance->GetRenderer();

            auto vertexShader = renderer->CreateShader({
                .type = Shader::Type::Vertex,
                .source = vertShaderSource
            });
            auto fragmentShader = renderer->CreateShader({
                .type = Shader::Type::Fragment,
                .source = fragShaderSource
            });
            // shader->
        },
        .onUpdate = [&testActor](double deltaTime)
        {
            testActor.Update(deltaTime);
        },
    });

    application.Run();

    return 0;
}
