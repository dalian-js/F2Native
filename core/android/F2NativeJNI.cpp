#include "JNIUtil.h"

#include <android/log.h>
#include <jni.h>

#include <AntGraphic.h>

#include "F2NativeJNI.h"
#include "JavaRef.h"

#include "android/JavaF2Function.h"
#include "graphics/XChart.h"
#include "graphics/geom/Geom.h"
#include "graphics/geom/Interval.h"
#include "graphics/guide/GuideController.h"

#ifndef xg_jni_arraysize

template <typename T, size_t N> char (&_xg_ArraySizeHelper(T (&array)[N]))[N];

#define xg_jni_arraysize(array) (sizeof(_xg_ArraySizeHelper(array)))
#endif // xg_jni_arraysize

// extern GCanvasSystemLog gcanvasSystemLog;

namespace xg {
namespace jni {

#if defined(TARGET_ALIPAY)

// static void NativeCanvasSwap(JNIEnv *env, jclass clazz, jlong view) {
//    ag::Canvas *canvas = reinterpret_cast<ag::Canvas *>(view);
//    canvas->swap();
//}
//
// static const JNINativeMethod native_canvas_methods[] = {
//        {
//    .name = "nSwap",
//    .signature = "(J)V",
//    .fnPtr = reinterpret_cast<void *>(NativeCanvasSwap),
//}
//};

#endif // TARGET_ALIPAY
static ScopedJavaGlobalRef<jclass> *gNativeCanvasProxyClass = nullptr;
static ScopedJavaGlobalRef<jclass> *gNativeChartProxyClass = nullptr;
static ScopedJavaGlobalRef<jclass> *gNativeFunctionProxyClass = nullptr;
static ScopedJavaGlobalRef<jclass> *gNativeF2CanvasViewProxyClass = nullptr;

//###################### F2Chart Begin ###################################
static jlong CreateNativeChart(JNIEnv *env, jclass clazz, jstring name, jdouble width, jdouble height, jdouble ratio) {
    std::string _name = JavaStringToString(env, name);
    F2_LOG_I("#CreateNativeChart", "create NativeChart:%s", _name.data());
    xg::XChart *chart = new xg::XChart(_name, width, height, ratio);
    return reinterpret_cast<jlong>(chart);
}

static jint SetChartCanvas(JNIEnv *env, jclass clazz, jlong chart, jlong view, jstring requestFrameHandleId, jboolean isAndroidCanvas) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    std::string _requestFrameHandleId = JavaStringToString(env, requestFrameHandleId);
    F2_LOG_I("#SetChartCanvas", "bind canvas requestFrameHandleId: %s", _requestFrameHandleId.data());
    _chart->SetRequestFrameFuncId(_requestFrameHandleId);
    F2_LOG_I("#CreateNativeChart", "isAndroidCanvas:%d",isAndroidCanvas);
    // 区分使用的CanvasContext
    if (isAndroidCanvas == JNI_TRUE){
        ScopedJavaGlobalRef<jobject> *handle = reinterpret_cast<ScopedJavaGlobalRef<jobject> *>(view);
        _chart->SetAndroidCanvasContext(handle->obj());
    } else {
      ag::Canvas *canvas = reinterpret_cast<ag::Canvas *>(view);
      auto ctx = (ag::CanvasRenderingContext2D *)canvas->getContext("2d");
      F2_LOG_I("#SetChartCanvas", "bind canvas ctx: %p", ctx);
      _chart->SetCanvasContext(ctx);
    }
    return 0;
}

static jint SetChartSource(JNIEnv *env, jclass clazz, jlong chart, jstring source) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    std::string data = JavaStringToString(env, source);
    _chart->Source(std::move(data));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartSource");
    return 0;
}

static jint SetChartPadding(JNIEnv *env, jclass clazz, jlong chart, jdouble left, jdouble top, jdouble right, jdouble bottom) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Padding(left, top, right, bottom);
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartPadding");
    return 0;
}

static jint SetChartMargin(JNIEnv *env, jclass clazz, jlong chart, jdouble left, jdouble top, jdouble right, jdouble bottom) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Margin(left, top, right, bottom);
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartMargin");
    return 0;
}

static jint SetChartScale(JNIEnv *env, jclass clazz, jlong chart, jstring field, jstring config) {
    std::string _field = JavaStringToString(env, field);
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Scale(std::move(_field), std::move(_config));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetScaleConfig");
    return 0;
}

static jint SetChartAxis(JNIEnv *env, jclass clazz, jlong chart, jstring field, jstring config) {
    std::string _field = JavaStringToString(env, field);
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Axis(std::move(_field), std::move(_config));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetAxisConfig");
    return 0;
}

static jint SetChartCoord(JNIEnv *env, jclass clazz, jlong chart, jstring config) {
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Coord(std::move(_config));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartCoord");
    return 0;
}

static jint SetChartAnimate(JNIEnv *env, jclass clazz, jlong chart, jstring config) {
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Animate(std::move(_config));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartAnimate");
    return 0;
}

static jint SetChartInteraction(JNIEnv *env, jclass clazz, jlong chart, jstring type, jstring config) {
    std::string _type = JavaStringToString(env, type);
    nlohmann::json cfg = nlohmann::json::parse(JavaStringToString(env, config), nullptr, false);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Interaction(_type, cfg);
    return 0;
}

static jint SetChartToolTip(JNIEnv *env, jclass clazz, jlong chart, jstring config) {
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Tooltip(std::move(_config));
    return 0;
}

static jint SetChartLegend(JNIEnv *env, jclass clazz, jlong chart, jstring field, jstring config) {
    std::string _field = JavaStringToString(env, field);
    std::string _config = JavaStringToString(env, config);

    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    _chart->Legend(_field, std::move(_config));
    F2_LOG_I(_chart->GetChartName(), "%s", "#SetAxisConfig");
    return 0;
}

static jint SetChartGuideType(JNIEnv *env, jclass clazz, jlong chart, jstring type, jstring config) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);

    std::string _type = JavaStringToString(env, type);
    std::string _config = JavaStringToString(env, config);
    if(_type == "text") {
        _chart->Guide().Text(std::move(_config));
        F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartGuideText");
    } else if(_type == "flag") {
        _chart->Guide().Flag(std::move(_config));
        F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartGuideFlag");
    } else if(_type == "line") {
        _chart->Guide().Line(std::move(_config));
        F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartGuideLine");
    } else if(_type == "background") {
        _chart->Guide().Background(std::move(_config));
        F2_LOG_I(_chart->GetChartName(), "%s", "#SetChartGuideBackground");
    }
    return 0;
}

static jint SendChartTouchEvent(JNIEnv *env, jclass clazz, jlong chart, jstring event) {
    std::string _event = JavaStringToString(env, event);
    bool ret = false;
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    ret = _chart->OnTouchEvent(std::move(_event));
    return ret ? 1 : 0;
}

static jlong CreateChartGeom(JNIEnv *env, jclass clazz, jlong chart, jstring type) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    std::string _type = JavaStringToString(env, type);
    xg::geom::AbstractGeom *geom = nullptr;
    if(_type == "line") {
        geom = &_chart->Line();
    } else if(_type == "area") {
        geom = &_chart->Area();
    } else if(_type == "interval") {
        geom = &_chart->Interval();
    } else if(_type == "point") {
        geom = &_chart->Point();
    } else if(_type == "candle") {
        geom = &_chart->Candle();
    }

    F2_LOG_I(_chart->GetChartName(), "createGeom: %s", _type.data());

    if(geom == nullptr) {
        return 0;
    }
    return reinterpret_cast<jlong>(geom);
}

static jstring GetChartRenderDumpInfo(JNIEnv *env, jclass clazz, jlong chart) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    F2_LOG_I(_chart->GetChartName(), "%s", "#:GetChartRenderDumpInfo");
    std::string dumpInfo = _chart->GetRenderInfo();
    return env->NewStringUTF(dumpInfo.data());
}

static jstring GetChartScaleTicks(JNIEnv *env, jclass clazz, jlong chart, jstring field) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    std::string _field = JavaStringToString(env, field);
    std::string ticks = _chart->GetScaleTicks(_field);
    return env->NewStringUTF(ticks.data());
}

static jint DestroyChart(JNIEnv *env, jclass clazz, jlong chart) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    F2_LOG_I(_chart->GetChartName(), "%s", "#destroy");
    delete _chart;
    return 0;
}

static jint ChartRender(JNIEnv *env, jclass clazz, jlong chart) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    F2_LOG_I(_chart->GetChartName(), "%s", "#render");
    _chart->Render();
    return 0;
}

static jdoubleArray ChartGetPosition(JNIEnv *env, jclass clazz, jlong chart, jstring itemData) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);

    std::string itemDataStr = JavaStringToString(env, itemData);
    nlohmann::json data = nlohmann::json::parse(itemDataStr, nullptr, false);
    const util::Point point = _chart->GetPosition(data);

    jdouble buf[] = {point.x, point.y};
    jdoubleArray rst = env->NewDoubleArray(2);
    env->SetDoubleArrayRegion(rst, 0, 2, &buf[0]);
    return rst;
}

static jstring ChartGetTooltipInfos(JNIEnv *env, jclass clazz, jlong chart, jfloat touchX, jfloat touchY, jint geomIndex) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    std::string tooltipInfo = _chart->GetTooltipInfos(touchX, touchY, geomIndex);
    F2_LOG_I(_chart->GetChartName(), "#:ChartGetTooltipInfos:got %s", tooltipInfo.c_str());
    return env->NewStringUTF(tooltipInfo.data());
}

static jint ChartClear(JNIEnv *env, jclass clazz, jlong chart) {
    xg::XChart *_chart = reinterpret_cast<xg::XChart *>(chart);
    F2_LOG_I(_chart->GetChartName(), "%s", "#clear");
    _chart->Clear();
    return 0;
}

static jint SetChartGeomPosition(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring fields) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _fields = JavaStringToString(env, fields);
    _geom->Position(_fields);
    return 0;
}

static jint SetChartGeomColors(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring field, jobjectArray colors) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _field = JavaStringToString(env, field);
    jsize colorsSize = env->GetArrayLength(colors);
    std::vector<std::string> _colors;
    for(int index = 0; index < colorsSize; ++index) {
        jstring color = (jstring)env->GetObjectArrayElement(colors, index);
        std::string _color = JavaStringToString(env, color);
        _colors.push_back(std::move(_color));
    }

    _geom->Color(_field, std::move(_colors));
    return 0;
}

static jint SetChartGeomColor(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring color) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _color = JavaStringToString(env, color);
    _geom->Color(_color);
    return 0;
}

static jint SetChartGeomSizes(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring field, jfloatArray sizes) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _field = JavaStringToString(env, field);
    std::vector<float> _sizes;
    jsize len = env->GetArrayLength(sizes);
    jfloat *array = env->GetFloatArrayElements(sizes, 0);
    for(int i = 0; i < len; ++i) {
        _sizes.push_back(array[i]);
    }
    _geom->Size(_field, std::move(_sizes));
    return 0;
}

static jint SetChartGeomSize(JNIEnv *env, jclass clazz, jlong geom, jstring type, jfloat size) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    _geom->Size(size);
    return 0;
}

static jint SetChartGeomIntervalTag(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring config) {
    std::string _type = JavaStringToString(env, type);
    if(_type != "interval") {
        return CODE_FAIL_UNKNOWN;
    }

    xg::geom::Interval *_interval = reinterpret_cast<xg::geom::Interval *>(geom);
    std::string _configStr = JavaStringToString(env, config);
    _interval->Tag(std::move(_configStr));
    return 0;
}

static jint SetChartGeomStyle(JNIEnv *env, jclass clazz, jlong geom, jstring config) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _configStr = JavaStringToString(env, config);
    _geom->Style(std::move(_configStr));
    return 0;
}

static jint SetChartGeomAttrs(JNIEnv *env, jclass clazz, jlong geom, jstring config) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _configStr = JavaStringToString(env, config);
    _geom->SetAttrs(std::move(_configStr));
    return 0;
}

static jint ExecuteCommand(JNIEnv *env, jclass clazz, jlong commandHandle) {
    xg::func::Command *command = reinterpret_cast<xg::func::Command *>(commandHandle);
    if(command != nullptr) {
        command->run();
        delete command;
    }
    return 0;
}

static jint DeallocCommand(JNIEnv *env, jclass clazz, jlong commandHandle) {
    xg::func::Command *command = reinterpret_cast<xg::func::Command *>(commandHandle);
    if(command != nullptr) {
        delete command;
    }
    return 0;
}

static jint SetChartGeomShapes(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring field, jobjectArray shapes) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _field = JavaStringToString(env, field);
    jsize len = env->GetArrayLength(shapes);
    std::vector<std::string> _shapes;
    for(int index = 0; index < len; ++index) {
        jstring shape = (jstring)env->GetObjectArrayElement(shapes, index);
        std::string _shape = JavaStringToString(env, shape);
        _shapes.push_back(std::move(_shape));
    }

    _geom->Shape(_field, std::move(_shapes));
    return 0;
}

static jint SetChartGeomShape(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring shape) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _shape = JavaStringToString(env, shape);
    _geom->Shape(_shape);
    return 0;
}

static jint SetChartGeomAdjust(JNIEnv *env, jclass clazz, jlong geom, jstring type, jstring adjust) {
    xg::geom::AbstractGeom *_geom = reinterpret_cast<xg::geom::AbstractGeom *>(geom);
    std::string _adjust = JavaStringToString(env, adjust);
    _geom->Adjust(_adjust);
    return 0;
}

static const JNINativeMethod native_chart_methods[] = {{
                                                           .name = "nCreateNativeChart",
                                                           .signature = "(Ljava/lang/String;DDD)J",
                                                           .fnPtr = reinterpret_cast<void *>(CreateNativeChart),
                                                       },
                                                       {
                                                           .name = "nSetCanvasView",
                                                           .signature = "(JJLjava/lang/String;Z)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartCanvas),
                                                       },
                                                       {
                                                           .name = "nSetSource",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartSource),
                                                       },
                                                       {
                                                           .name = "nSetPadding",
                                                           .signature = "(JDDDD)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartPadding),
                                                       },
                                                       {
                                                           .name = "nSetMargin",
                                                           .signature = "(JDDDD)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartMargin),
                                                       },
                                                       {
                                                           .name = "nSetScale",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartScale),
                                                       },
                                                       {
                                                           .name = "nSetAxis",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartAxis),
                                                       },
                                                       {
                                                           .name = "nSetCoord",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartCoord),
                                                       },
                                                       {
                                                           .name = "nSetAnimate",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartAnimate),
                                                       },
                                                       {
                                                           .name = "nSetInteraction",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartInteraction),
                                                       },
                                                       {
                                                           .name = "nSetToolTip",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartToolTip),
                                                       },
                                                       {
                                                           .name = "nSetLegend",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartLegend),
                                                       },
                                                       {
                                                           .name = "nSetGuideType",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGuideType),
                                                       },
                                                       {
                                                           .name = "nSendTouchEvent",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SendChartTouchEvent),
                                                       },
                                                       {
                                                           .name = "nCreateGeom",
                                                           .signature = "(JLjava/lang/String;)J",
                                                           .fnPtr = reinterpret_cast<void *>(CreateChartGeom),
                                                       },
                                                       {
                                                           .name = "nGetRenderDumpInfo",
                                                           .signature = "(J)Ljava/lang/String;",
                                                           .fnPtr = reinterpret_cast<void *>(GetChartRenderDumpInfo),
                                                       },
                                                       {
                                                           .name = "nGetScaleTicks",
                                                           .signature = "(JLjava/lang/String;)Ljava/lang/String;",
                                                           .fnPtr = reinterpret_cast<void *>(GetChartScaleTicks),
                                                       },
                                                       {
                                                           .name = "nDestroy",
                                                           .signature = "(J)I",
                                                           .fnPtr = reinterpret_cast<void *>(DestroyChart),
                                                       },
                                                       {
                                                           .name = "nRender",
                                                           .signature = "(J)I",
                                                           .fnPtr = reinterpret_cast<void *>(ChartRender),
                                                       },
                                                       {
                                                           .name = "nGetPosition",
                                                           .signature = "(JLjava/lang/String;)[D",
                                                           .fnPtr = reinterpret_cast<void *>(ChartGetPosition),
                                                       },
                                                       {
                                                           .name = "nGetTooltipInfos",
                                                           .signature = "(JFFI)Ljava/lang/String;",
                                                           .fnPtr = reinterpret_cast<void *>(ChartGetTooltipInfos),
                                                       },
                                                       {
                                                           .name = "nClear",
                                                           .signature = "(J)I",
                                                           .fnPtr = reinterpret_cast<void *>(ChartClear),
                                                       },
                                                       {
                                                           .name = "nGeomPosition",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomPosition),
                                                       },
                                                       {
                                                           .name = "nGeomColor",
                                                           .signature =
                                                               "(JLjava/lang/String;Ljava/lang/String;[Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomColors),
                                                       },
                                                       {
                                                           .name = "nGeomColor",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomColor),
                                                       },
                                                       {
                                                           .name = "nGeomSize",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;[F)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomSizes),
                                                       },
                                                       {
                                                           .name = "nGeomSize",
                                                           .signature = "(JLjava/lang/String;F)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomSize),
                                                       },
                                                       {
                                                           .name = "nGeomShape",
                                                           .signature =
                                                               "(JLjava/lang/String;Ljava/lang/String;[Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomShapes),
                                                       },
                                                       {
                                                           .name = "nGeomShape",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomShape),
                                                       },
                                                       {
                                                           .name = "nGeomAdjust",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomAdjust),
                                                       },
                                                       {
                                                           .name = "nGeomIntervalTag",
                                                           .signature = "(JLjava/lang/String;Ljava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomIntervalTag),
                                                       },
                                                       {
                                                           .name = "nGeomStyle",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomStyle),
                                                       },
                                                       {
                                                           .name = "nGeomAttrs",
                                                           .signature = "(JLjava/lang/String;)I",
                                                           .fnPtr = reinterpret_cast<void *>(SetChartGeomAttrs),
                                                       },
                                                       {
                                                           .name = "nExecuteCommand",
                                                           .signature = "(J)I",
                                                           .fnPtr = reinterpret_cast<void *>(ExecuteCommand),
                                                       },
                                                       {
                                                           .name = "nDeallocCommand",
                                                           .signature = "(J)I",
                                                           .fnPtr = reinterpret_cast<void *>(DeallocCommand),
                                                       }};
//###################### F2Chart END ###################################

static ScopedJavaGlobalRef<jclass> *gF2LogClass = nullptr;
static jmethodID gF2LogInfoMethod = nullptr;
static jmethodID gF2LogWarnMethod = nullptr;
static jmethodID gF2LogErrorMethod = nullptr;

static bool InitF2Log(JNIEnv *env) {
    if(gF2LogClass != nullptr) {
        return true;
    }

    jclass logClass = env->FindClass("com/antgroup/antv/f2/F2Log");

    if(logClass == nullptr) {
        return false;
    }

    gF2LogInfoMethod = env->GetStaticMethodID(logClass, "i", "(Ljava/lang/String;Ljava/lang/String;)V");
    gF2LogWarnMethod = env->GetStaticMethodID(logClass, "w", "(Ljava/lang/String;Ljava/lang/String;)V");
    gF2LogErrorMethod = env->GetStaticMethodID(logClass, "e", "(Ljava/lang/String;Ljava/lang/String;)V");
    if(HasException(env)) {
        return false;
    }

    gF2LogClass = new ScopedJavaGlobalRef<jclass>(env, logClass);

    return true;
}
//###################### F2Function Begin ###################################

static jstring nCreateFunction(JNIEnv *env, jclass clazz, jobject jhandle) {
    ScopedJavaGlobalRef<jobject> *handle = new ScopedJavaGlobalRef<jobject>(env, jhandle);
    xg::func::F2Function *function = new xg::jni::JavaF2Function(handle);
    xg::func::FunctionManager::GetInstance().Add(function);
    return StringToJString(env, function->functionId);
}

static void nFunctionBindChart(JNIEnv *env, jclass clazz, jstring jfunctionId, jlong jchart) {
    std::string functionId = JavaStringToString(env, jfunctionId);
    F2_LOG_I("#nFunctionBindChart", "functionId: %s", functionId.data());
    xg::func::F2Function *function = xg::func::FunctionManager::GetInstance().Find(functionId);

    if(function != nullptr) {
        xg::XChart *_chart = reinterpret_cast<xg::XChart *>(jchart);
        function->hostChartId = _chart->GetChartId();
    }
}

static const JNINativeMethod native_function_methods[] = {{
                                                              .name = "nCreateFunction",
                                                              .signature = "(Ljava/lang/Object;)Ljava/lang/String;",
                                                              .fnPtr = reinterpret_cast<void *>(nCreateFunction),
                                                          },
                                                          {
                                                              .name = "nBindChart",
                                                              .signature = "(Ljava/lang/String;J)V",
                                                              .fnPtr = reinterpret_cast<void *>(nFunctionBindChart),
                                                          }};

//###################### F2Function End ###################################

//###################### F2CanvasView Start ###################################
static jlong nCreateCanvasContextHandle(JNIEnv *env, jclass clazz, jobject canvasConext) {
    //因为canvasConext是个临时对象，使用ScopedJavaGlobalRef包裹，确保canvasConext不被释放
    ScopedJavaGlobalRef<jobject> *handle = new ScopedJavaGlobalRef<jobject>(env, canvasConext);
    return reinterpret_cast<jlong>(handle);
}

static void nDestroyCanvasContextHandle(JNIEnv *env, jclass clazz, jlong scopedGlobalJavaRef) {
    //释放scopedGlobalJavaRef对象
    ScopedJavaGlobalRef<jobject> *handle = reinterpret_cast<ScopedJavaGlobalRef<jobject> *>(scopedGlobalJavaRef);
    delete handle;
    handle = nullptr;
}

static const JNINativeMethod native_canvascontext_methods[] = {{
                                                                   .name = "nCreateCanvasContextHandle",
                                                                   .signature = "(Ljava/lang/Object;)J",
                                                                   .fnPtr = reinterpret_cast<void *>(nCreateCanvasContextHandle),
                                                               },
                                                               {
                                                                   .name = "nDestroyCanvasContextHandle",
                                                                   .signature = "(J)V",
                                                                   .fnPtr = reinterpret_cast<void *>(nDestroyCanvasContextHandle),
                                                               }};
//###################### F2CanvasView End ###################################

static bool
RegisterJNIInterface(JNIEnv *env, ScopedJavaGlobalRef<jclass> **holder, const char *class_path, const JNINativeMethod *methods, int array_size) {
    jclass clazz = env->FindClass(class_path);
    (*holder) = new ScopedJavaGlobalRef<jclass>(env, clazz);
    if((*holder)->isNull()) {
        F2_LOG_E("#RegisterJNIInterface", "can't find class %s", class_path);
        return false;
    }
    if(env->RegisterNatives((*holder)->obj(), methods, array_size) != 0) {
        F2_LOG_E("#RegisterJNIInterface", "%s", "Register JNI methods failed.");
        return false;
    }
    F2_LOG_I("#RegisterJNIInterface", "%s", "Register jni success");
    return true;
}

void InnerLog(int level, std::string traceId, const char *fmt, ...) {
    std::string _tag = "jni|" + traceId;

    char msg[2048] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, 2048, fmt, args);
    va_end(args);

    jmethodID method = gF2LogInfoMethod;
    if(level == F2_LOG_WARN) {
        method = gF2LogWarnMethod;
    } else if(level == F2_LOG_ERROR) {
        method = gF2LogErrorMethod;
    }

    if(gF2LogClass == nullptr || gF2LogClass->isNull() || method == nullptr) {
        native_clog(level, _tag.data(), msg);
        return;
    }

    JNIEnv *env = GetJniEnvSafe();
    if(env == nullptr) {
        return;
    }
    ScopedJavaLocalRef<jstring> tagStr = StringToJavaString(env, _tag);
    ScopedJavaLocalRef<jstring> msgStr = StringToJavaString(env, msg);
    env->CallStaticVoidMethod(gF2LogClass->obj(), method, tagStr.obj(), msgStr.obj());
}

static bool OnJniLoad(JNIEnv *env) {
    if(!InitF2Log(env)) {
        return false;
    }

    F2_LOG_I("#OnJniLoad", "%s", "init log success");

    if(!RegisterJNIInterface(env, &gNativeF2CanvasViewProxyClass, "com/antgroup/antv/f2/F2AndroidCanvasView",
                             native_canvascontext_methods, xg_jni_arraysize(native_canvascontext_methods))) {
        return false;
    }
    F2_LOG_I("#OnJniLoad", "%s", "register canvas view success");

    if(!RegisterJNIInterface(env, &gNativeChartProxyClass, "com/antgroup/antv/f2/NativeChartProxy", native_chart_methods,
                             xg_jni_arraysize(native_chart_methods))) {
        return false;
    }
    F2_LOG_I("#OnJniLoad", "%s", "register f2 chart success");

    if(!RegisterJNIInterface(env, &gNativeFunctionProxyClass, "com/antgroup/antv/f2/F2Function", native_function_methods,
                             xg_jni_arraysize(native_function_methods))) {
        return false;
    }
    F2_LOG_I("#OnJniLoad", "%s", "register f2function success");
    return true;
}

} // namespace jni
} // namespace xg

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    F2_LOG_I("#JNI_OnLoad", "%s", "JNI_OnLoad Start...");
    JavaVM *old = xg::jni::GetJVM();
    if(old == vm) {
        F2_LOG_I("#JNI_OnLoad", "%s", "JNI_OnLoad dupliation vm, skip OnLoad");
        return JNI_VERSION_1_4;
    }

    xg::jni::InitJavaVM(vm);

    JNIEnv *env = xg::jni::AttachCurrentThread();
    F2_LOG_I("#JNI_OnLoad", "%s", "AttachCurrentThread FINISH");
    bool result = xg::jni::OnJniLoad(env);

    if(!result) {
        F2_LOG_E("#JNI_OnLoad", "%s", "JNI_OnLoad FAIL");
    }
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) { F2_LOG_I("#JNI_OnUnload", "%s", "JNI_OnUnload Finished"); }
