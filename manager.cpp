#include "manager.h"

#include <QDebug>

#ifdef Q_OS_WASM
#include <emscripten.h>
#include <emscripten/html5.h>

//这个EM_JS是用来声明js函数的，声明之后就可以直接在c++里面用了。

// 获取网页网址
EM_JS(const char*, getTitle, (), {
          var urlPath = window.document.location.href;
          console.log(urlPath);

          var jstring = urlPath;

          var lengthBytes = lengthBytesUTF8(jstring) + 1;
          var stringOnWasmHeap = _malloc(lengthBytes);
          stringToUTF8(jstring, stringOnWasmHeap, lengthBytes);
          return stringOnWasmHeap;
      })

// 输入中文方法一
// 获取用户输入，因为Qt不支持直接输入中文。（这个是弹窗获取输入的，比较影响用户体验，不太建议使用）
EM_JS(const char*, getInput, (const char *str), {
          const text = UTF8ToString(str);
          val = prompt("", text);

          if(val == null)
          {
              val = ""
          }
          var jstring = val;

          var lengthBytes = lengthBytesUTF8(jstring) + 1;
          var stringOnWasmHeap = _malloc(lengthBytes);
          stringToUTF8(jstring, stringOnWasmHeap, lengthBytes);
          return stringOnWasmHeap;
      })


// 输入中文方法二
// 创建输入框，用于覆盖原始的qml输入框。创建的输入框的id为zyInput
EM_JS(void, createInputX, (const char *str, const int x, const int y, const int width, const int height), {
          const text = UTF8ToString(str);

          var muiDiv = window.document.getElementById('qtcanvas');
          createInput('txtInput','text', text, muiDiv.parentElement);

          console.log("getInputX", muiDiv, muiDiv.parentElement);

          function createInput(inputName, inputType, inputValue, aDiv) {
            var input = window.document.createElement("input");
            input.setAttribute("type", inputType);
            input.setAttribute("name", inputName);
            input.setAttribute("value", inputValue);

            var style = "border:0px solid groove; position:absolute;  margin: 0px; padding: 0px; z-index: 10000; left:" + x
              + "px; top:" + y
              + "px; width:" + width
              + "px; height:" + height
              + "px; opacity:1.0";

            console.log("style", style);
            input.setAttribute("style", style);

            input.setAttribute("id", "zyInput"); // 设置id
            input.setAttribute("onBlur", "removeInput()");  // 设置回调函数

            console.log("id", input.id);

            aDiv.appendChild(input);
            console.log("createInput");
            input.focus();
          }
      })

// 注入函数，使其成为qtcanvas的一个script函数；
// 注入的函数removeInput的作用是移除zyInput控件
EM_JS(void, injectJs, (), {
          var sObj = window.document.createElement("script");
          sObj.setAttribute("type", "text/javascript");
          sObj.setAttribute("id", "zyS");
          sObj.innerHTML = "
          function removeInput()
          {
              let obj = window.document.getElementById('zyInput');
              obj.parentNode.removeChild(obj);
              console.log('input value', obj.value);
          }";

          console.log("sObj", sObj.innerHTML, sObj.type);

          var muiDiv = window.document.getElementById('qtcanvas');
          muiDiv.appendChild(sObj);
      })

//获取输入控件的输入值
EM_JS(const char*, getText, (), {
          let obj = window.document.getElementById('zyInput');
          var jstring = "";
          if(obj !== null)
          {
              jstring = obj.value;
          }

          var lengthBytes = lengthBytesUTF8(jstring) + 1;
          var stringOnWasmHeap = _malloc(lengthBytes);
          stringToUTF8(jstring, stringOnWasmHeap, lengthBytes);
          return stringOnWasmHeap;
      })

// 使输入控件主动失去焦点，因为在触摸屏下，不会主动失去焦点
EM_JS(void, deFocus, (), {
          let obj = window.document.getElementById('zyInput');
          if(obj !== null)
          {
             obj.blur()
          }
      })


// 用来调试的函数，事件的类型可以查看  <emscripten/html5.h>
// 这段代码我是从网上找的，具体哪里忘记了
static inline const char *emscripten_event_type_to_string(int eventType) {
  const char *events[] = { "(invalid)", "(none)", "keypress", "keydown", "keyup", "click", "mousedown", "mouseup", "dblclick", "mousemove", "wheel", "resize",
    "scroll", "blur", "focus", "focusin", "focusout", "deviceorientation", "devicemotion", "orientationchange", "fullscreenchange", "pointerlockchange",
    "visibilitychange", "touchstart", "touchend", "touchmove", "touchcancel", "gamepadconnected", "gamepaddisconnected", "beforeunload",
    "batterychargingchange", "batterylevelchange", "webglcontextlost", "webglcontextrestored", "mouseenter", "mouseleave", "mouseover", "mouseout", "(invalid)" };

  ++eventType;
  if (eventType < 0)
      eventType = 0;
  if (eventType >= sizeof(events)/sizeof(events[0]))
  {
      eventType = sizeof(events)/sizeof(events[0])-1;
  }

  return events[eventType];
}

// 控件失去焦点时的回调函数
EM_BOOL focusevent_callback(int eventType, const EmscriptenFocusEvent *e, void *userData)
{
//    printf("%s, nodeName: \"%s\", id: \"%s\"\n",
//           emscripten_event_type_to_string(eventType),
//           e->nodeName,
    //           e->id[0] == '\0' ? "(empty string)" : e->id);

    QString itemID = QString(e->id);
    if(itemID == "zyInput")
    {
        QString inputText = getText();

        Manager *man = (Manager*)userData;
        man->setInputText(inputText);
    }

    return 0;
}
#endif

Manager::Manager(QObject *parent)
    : QObject{parent}
{
    mTextInput = nullptr;

#ifdef Q_OS_WASM
    injectJs();

    // 注册 控件失去焦点 回调函数
    auto ret = emscripten_set_blur_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, focusevent_callback);

    qDebug() << "register blur_callback:" << ret;
#else

#endif

}

int Manager::showTextInput(QObject* item, QString currentText, int x, int y, int width, int height)
{
    mTextInput = item;

#ifdef Q_OS_WASM
    createInputX(currentText.toUtf8().data(), x, y, width, height);
#endif

    return 0;
}

int Manager::setInputText(QString text)
{
    if(mTextInput == nullptr)
    {
        return -1;
    }

    mTextInput->setProperty("text", text);

    return 0;
}
