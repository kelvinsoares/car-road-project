#ifndef TRACK_H
#define TRACK_H

#include <GL/gl.h>

typedef struct {
    float radiusInner;
    float radiusOuter;
    float height;
    int segments;
    float length;
    GLuint textureID;
} OvalTrack;

void initTrack(OvalTrack* track, float inner, float outer, float height, int segments, float length);
void drawOvalTrack(const OvalTrack* track);
GLuint loadTrackTexture(const char* filename);

#endif
