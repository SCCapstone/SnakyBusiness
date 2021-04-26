#include "undoRedo.h"

// Change Vector Width
ChangeWidth::ChangeWidth(DataIOHandler *vec, const int &oldWidth, const int &width, QUndoCommand *parent) : QUndoCommand(parent) {
   // default
   wVec = vec;
   prevWidth = oldWidth;
   newWidth = width;
}
void ChangeWidth::undo() {
    wVec->getWorkingLayer()->setWidth(prevWidth);
}
void ChangeWidth::redo() {
    wVec->getWorkingLayer()->setWidth(newWidth);
}
// Change Vector Taper1
ChangeVectorTaper1::ChangeVectorTaper1(DataIOHandler *vec, const int &oldTaper, const int &taper, QUndoCommand *parent) : QUndoCommand(parent) {
    tVec = vec;
    prevTaper = oldTaper;
    newTaper = taper;
}
void ChangeVectorTaper1::undo() {
    tVec->getWorkingLayer()->setVectorTaper1(prevTaper);
}
void ChangeVectorTaper1::redo() {
    tVec->getWorkingLayer()->setVectorTaper1(newTaper);
}
// Change Vector Taper2
ChangeVectorTaper2::ChangeVectorTaper2(DataIOHandler *vec, const int &oldTaper, const int &taper, QUndoCommand *parent) : QUndoCommand(parent) {
    tVec = vec;
    prevTaper = oldTaper;
    newTaper = taper;
}
void ChangeVectorTaper2::undo() {
    tVec->getWorkingLayer()->setVectorTaper2(prevTaper);
}
void ChangeVectorTaper2::redo() {
    tVec->getWorkingLayer()->setVectorTaper2(newTaper);
}
//Change Vector Filter Strength

//Paste Vector
PasteVector::PasteVector(DataIOHandler *vec, QUndoCommand *parent) : QUndoCommand(parent) {
    pVec = vec;
}
void PasteVector::undo() {
    pVec->cutVectors();
}
void PasteVector::redo() {
    pVec->pasteVectors();
}
// Change Vector Filter Strength
ChangeVectorRange::ChangeVectorRange(DataIOHandler *vec, const int &oldStrength, const int &strength, QUndoCommand *parent) : QUndoCommand(parent) {
    rVec = vec;
    prevStrength = oldStrength;
    newStrength = strength;
}
void ChangeVectorRange::undo() {
    rVec->getWorkingLayer()->setVectorFilterStrength(prevStrength);
}
void ChangeVectorRange::redo() {
    rVec->getWorkingLayer()->setVectorFilterStrength(newStrength);
}
// Change Filter Strength
ChangeFilterRange::ChangeFilterRange(DataIOHandler *vec, const int &oldStrength, const int &strength, QUndoCommand *parent) : QUndoCommand(parent) {
    rVec = vec;
    prevStrength = oldStrength;
    newStrength = strength;
}
void ChangeFilterRange::undo() {
    rVec->getWorkingLayer()->setFilterStrength(prevStrength);
}
void ChangeFilterRange::redo() {
    rVec->getWorkingLayer()->setFilterStrength(newStrength);
}
// Insert Layer
InsertLayer::InsertLayer(DataIOHandler *vec, QUndoCommand *parent) : QUndoCommand(parent) {
    myVec = vec;
}
void InsertLayer::undo() {
    myVec->deleteLayer();
}
void InsertLayer::redo() {
    myVec->copyLayer();
    myVec->pasteLayer();
}
// Delete Layer
DeleteLayer::DeleteLayer(DataIOHandler *vec, QUndoCommand *parent) : QUndoCommand(parent) {
    myVec = vec;
}
void DeleteLayer::undo() {
    myVec->copyLayer();
    myVec->deleteLayer();
}
void DeleteLayer::redo() {
    myVec->pasteLayer();
}
// Move Layer Forward
MoveLayerForward::MoveLayerForward(DataIOHandler *vec, QUndoCommand *parent) : QUndoCommand(parent) {
    mVec = vec;
}
void MoveLayerForward::undo() {
    mVec->moveBackward();
}
void MoveLayerForward::redo() {
    mVec->moveForward();
}
// Move Layer To Front
MoveLayerToFront::MoveLayerToFront(DataIOHandler *vec, const int &oldPos, QUndoCommand *parent) : QUndoCommand(parent) {
    myVec = vec;
    prevPos = oldPos;
}
void MoveLayerToFront::undo() {
    int pos = myVec->getActiveLayer();
    while (pos > prevPos) {
        myVec->moveBackward();
        pos--;
    }
}
void MoveLayerToFront::redo() {
    int pos = myVec->getActiveLayer();
    while (pos < prevPos) {
        myVec->moveForward();
        pos++;
    }
}
// Move Layer Backward
MoveLayerBackward::MoveLayerBackward(DataIOHandler *vec, QUndoCommand *parent) : QUndoCommand(parent) {
    mVec = vec;
}
void MoveLayerBackward::undo() {
    mVec->moveForward();
}
void MoveLayerBackward::redo() {
    mVec->moveBackward();
}
// Move Layer To Back
MoveLayerToBack::MoveLayerToBack(DataIOHandler *vec, const int &oldPos, QUndoCommand *parent) : QUndoCommand(parent) {
    myVec = vec;
    prevPos = oldPos;
}
void MoveLayerToBack::undo() {
    int pos = myVec->getActiveLayer();
    while (pos < prevPos) {
        myVec->moveForward();
        pos++;
    }
}
void MoveLayerToBack::redo() {
    int pos = myVec->getActiveLayer();
    while (pos > prevPos) {
        myVec->moveToBack();
        pos--;
    }
}

// Change Layer
changeActiveLayer::changeActiveLayer(DataIOHandler *vec, const int &oldLayer, const int &layer, EditMode mode, QUndoCommand *parent) : QUndoCommand(parent) {
    lVec = vec;
    prevLayer = oldLayer;
    newLayer = layer;
    lMode = mode;
}
void changeActiveLayer::undo() {
    lVec->getWorkingLayer()->deselect();
    lVec->setActiveLayer(prevLayer, lMode);
}
void changeActiveLayer::redo() {
    lVec->getWorkingLayer()->deselect();
}
// Un-implememted
// Brush Undo
/*
BrushUndo::BrushUndo(brushHandler bh, list <Triple> l, QUndoCommand *parent) : QUndoCommand(parent) {
    myBrush = bh;
    trip = l;

}
void BrushUndo::undo() {
    // display original canvas before the brush
    BrushAction b (true, true);
    if(b.hasStopped(true)) {

    }
}
void BrushUndo::redo() {
    // update canvas to the new

}
RadialBrushUndo::RadialBrushUndo(brushHandler *bh, QPoint qp, vector<vector<QRgb>> colors, QUndoCommand *parent): QUndoCommand(parent) {
    myBrush = bh;
    myPoint = qp;
    deltaColors = colors;
}
void RadialBrushUndo::undo() {

}
void RadialBrushUndo::redo() {

}
// Create Vector
CreateVector::CreateVector(DataIOHandler *vec, const QPoint &a, const MouseButton &button, QUndoCommand *parent) : QUndoCommand(parent) {
    cVec = vec;
    cButton = button;
    cP = a;
}
void CreateVector::undo() {
    // if one point is added, undo

    // if a vector/one point is created, undo
}
void CreateVector::redo() {
    // if vector has less than 2 points, create vector

    // if the vector has more than 2 points, add a point
}
// Delete Vector
DeleteVector::DeleteVector(DataIOHandler *vec, QPoint a, QUndoCommand *parent) : QUndoCommand(parent) {
    dVec = vec;
    dP = a;
}
void DeleteVector::undo() {

}
void DeleteVector::redo() {

}
TranslateVector::TranslateVector(Layer *vec, const QPoint &prevPos, const QPoint &pos, QUndoCommand *parent) : QUndoCommand(parent) {
    tVec = vec;
    oldPos = prevPos;
    newPos = pos;
}
void TranslateVector::undo() {
}
void TranslateVector::redo() {

}
// Rotate Vector
RotateVec::RotateVec(Layer *vec, const QPoint &pos, QUndoCommand *parent) : QUndoCommand(parent) {
    myVec = vec;
    aP = pos;
}
void RotateVec::undo() {
    myVec->moveRight(aP);
}
void RotateVec::redo() {
    myVec->moveRight(aP);
}*/
