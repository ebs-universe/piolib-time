
#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#ifndef PIO_NATIVE

#ifdef __cplusplus
extern "C"
{
#endif

#define UNITY_SUPPORT_64

void unityOutputStart();
void unityOutputChar(char);
void unityOutputFlush();
void unityOutputComplete();

#define UNITY_OUTPUT_START()    unityOutputStart()
#define UNITY_OUTPUT_CHAR(c)    unityOutputChar(c)
#define UNITY_OUTPUT_FLUSH()    unityOutputFlush()
#define UNITY_OUTPUT_COMPLETE() unityOutputComplete()

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
#endif /* UNITY_CONFIG_H */