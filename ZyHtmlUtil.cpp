#include "ZyHtmlUtil.h"

#include <QDebug>
#include <QVariant>
#include <QFileDialog>

#ifdef Q_OS_WASM
#include <emscripten.h>
#include <emscripten/html5.h>


// 这个EM_JS是用来声明js函数的，声明之后就可以直接在c++里面调用了。
// 而且不仅可以在c++用，也可以在网页js里面用
// 而 EMSCRIPTEN_KEEPALIVE 声明的函数则可以在网页里面通过 Module.ccall来调用


// 创建输入框，用于覆盖原始的qml输入框。创建的输入框的id以"zyInput_"开头
EM_JS(void, createInputX, (quintptr ptr, const char *str, const int x, const int y, const int width, const int height), {
    const text = UTF8ToString(str);

    var canvasName = 'qtcanvas';
#if QT_VERSION_MAJOR >= 6
    canvasName = 'screen';
#endif

    var canvas = window.document.getElementById(canvasName);

    var form = canvas.parentElement;

    var input = window.document.createElement('input');
    input.setAttribute('type', 'text');
    input.setAttribute('name', 'txtInput');
    input.setAttribute('value', text);

    // 这样设置位置、尺寸，才能让控件“浮”在最上面
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

// 读取本地电脑的文件，可多选
EM_JS(void, readLocalFileX, (quintptr ptr), {
    const input = document.createElement('input');
    input.type = 'file';
    input.webkitdirectory = true;
    input.directory = true;
//    input.multiple = "multiple";

    input.addEventListener('change',() => {
            // 获取文件列表
            const files = input.files;
            for (let i = 0; i < files.length; i++)
            {
                let reader = new FileReader();
                reader.readAsArrayBuffer(files[i]);
                reader.onload = () => {
                    var result = reader.result;
                    console.log("filename", files[i].name, files[i].webkitRelativePath);

                    return;

                    const arr = new Uint8Array(result);
                    var length = arr.length;

                    // 获取 Uint8Array 数据的底层内存地址
                    const dataPtr = Module._malloc(arr.length);
                    const dataOffset = dataPtr / Module.HEAPU8.BYTES_PER_ELEMENT;
                    const dataHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, arr.length);
                    dataHeap.set(arr);

                    console.log("uint8Array", dataOffset, length);
                    // 调用 C++ 函数
                    Module.ccall('processFile',
                                 'number',
                                 ['number', 'string', 'number', 'number'],
                                 [ptr, files[i].name, dataOffset, length]);

                    // 释放分配的内存
                    Module._free(dataPtr);
                }

            }
        });

    input.click();
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

// 处理选择的文件，文件已经被读取，此处直接处理读取后的文件数据
EMSCRIPTEN_KEEPALIVE
    int processFile(quintptr ptr, const char *fileName, const char *data, size_t dataSize)
{
    qDebug() << "process file:" << ptr << fileName;
    qDebug() << "data:" << dataSize << (uint8_t*)data;

    QByteArray dataArray(data, dataSize);
    qDebug() << "dataArray size:" << dataArray.size();

    // 该函数可以调用浏览器api下载保存数据
    QFileDialog::saveFileContent(dataArray, QString(fileName));

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

// 跨域的cookie测试
EM_JS(void, corsTest, (), {
          var xhr = new XMLHttpRequest();
          xhr.open('GET', 'http://www.jsontest.com', true);
          xhr.withCredentials = true; //设置withCredentials选项为true
//          xhr.onload = function() {
//              if (xhr.status === 200) {
//                  console.log(xhr.responseText);
//              }
//          };
          xhr.onreadystatechange = function() {
            if (this.readyState == this.HEADERS_RECEIVED) {
              console.log("headers:", xhr.getAllResponseHeaders());
            }
          };
          xhr.send();
      })

// 创建iframe,并显示
EM_JS(void, createIframe, (const char *urlText, int x, int y, int width, int height), {

    var url = UTF8ToString(urlText);

    var iframe = document.createElement('iframe');
    iframe.src = url;
    iframe.scrolling = 'yes';

    // 调整 Iframe 的大小和位置
    var style = 'border:medium double rgb(250,0,255); position:absolute;  margin: 0px; padding: 0px; z-index: 10000; left:' + x
                + 'px; top:' + y
                + 'px; width:' + width
                + 'px; height:' + height
                + 'px; opacity:1.0';

    iframe.style = style;
    document.body.appendChild(iframe);
})

#endif

ZyHtmlUtil::ZyHtmlUtil(QObject *parent)
    : QObject{parent}
{

}

int ZyHtmlUtil::showTextInput(QObject* item, QString currentText, int x, int y, int width, int height)
{

#ifdef Q_OS_WASM
    blurAllInput();
    createInputX((quintptr)item, currentText.toUtf8().data(), x, y, width, height);
#endif

    return 0;
}

int ZyHtmlUtil::functionTest(quintptr ptr)
{

#ifdef Q_OS_WASM
    //    corsTest();
    //    readLocalFileX(ptr);
    createIframe("https://www.bing.com", 100, 100, 500, 300);
#endif

    return 0;
}

