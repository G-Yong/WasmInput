#include "ZyHtmlUtil.h"

#include <QDebug>

#ifdef Q_OS_WASM
#include <emscripten.h>
#include <emscripten/html5.h>

// 这个EM_JS是用来声明js函数的，声明之后就可以直接在c++里面调用了。
// 而且不仅可以在c++用，也可以在网页js里面用
// 而 EMSCRIPTEN_KEEPALIVE 声明的函数则可以在网页里面通过 Module.ccall来调用


// 创建输入框，用于覆盖原始的qml输入框。创建的输入框的id为zyInput
EM_JS(void, createInputX, (quintptr ptr, const char *str, const int x, const int y, const int width, const int height), {
          const text = UTF8ToString(str);

          var canvas = window.document.getElementById('qtcanvas');
          var form = canvas.parentElement;

          var input = window.document.createElement('input');
          input.setAttribute('type', 'text');
          input.setAttribute('name', 'txtInput');
          input.setAttribute('value', text);

          var style = 'border:0px solid groove; position:absolute;  margin: 0px; padding: 0px; z-index: 10000; left:' + x
          + 'px; top:' + y
          + 'px; width:' + width
          + 'px; height:' + height
          + 'px; opacity:1.0';

          input.setAttribute('style', style);

          input.setAttribute('id', 'zyInput_' + ptr); // 设置id
          input.onblur = function(){
              form.removeChild(input);
          };  // 失去焦点时，就移除自身
          input.onchange = function(){
              // 调用 C 函数, 将文字拷贝到Qt的控件
              var result = Module.ccall('setWidgetText', // 函数名称
                                        'number',    // 返回值类型
                                        ['number', 'string'], // 参数类型
                                        [ptr, input.value]);   // 参数列表
          }; //文字发生更改时，修改Qt控件的值

          form.appendChild(input);

          input.focus();
//          console.log("createInputX complete", input.id, x, y, width, height);
      })

extern "C"{
EMSCRIPTEN_KEEPALIVE
int setWidgetText(quintptr ptr, const char *text)
{
//    qDebug() << "setWidgetText" << ptr << text;

    QObject *obj = (QObject*)ptr;
    obj->setProperty("text", text);

    return 0;
}
}

// 对所有生成的input进行blur处理
EM_JS(void, blurAllInput, (), {
          // 动态的 HTMLCollection 对象的问题，因此需要先转成数组在遍历
          let inputList = Array.from(document.getElementsByTagName('input'));
          for(let i = 0; i < inputList.length; i++){
              let input = inputList[i];
              if(input && input.parentElement && input.id.startsWith('zyInput_'))
              {
                  input.blur()
              }
          }
      })


#endif

class ClassA
{
public:
    ClassA() {}
};


ZyHtmlUtil::ZyHtmlUtil(QObject *parent)
    : QObject{parent}
{
}

int ZyHtmlUtil::showTextInput(QObject* item, QString currentText, int x, int y, int width, int height)
{
    blurAllInput();

#ifdef Q_OS_WASM
    createInputX((quintptr)item, currentText.toUtf8().data(), x, y, width, height);
#endif

    return 0;
}

