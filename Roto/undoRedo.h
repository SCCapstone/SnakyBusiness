#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QUndoCommand>
#include <QAction>
#include <dataIOHandler.h>
#include <layer.h>
#include <brushhandler.h>
// sample Undo Functionality

// Brush Undo
class BrushUndo : public QUndoCommand {
public:
    BrushUndo(brushHandler brush, DataIOHandler *qi, const QPoint &qp, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    brushHandler myBrush;
    DataIOHandler *myQ;
    QPoint myPoint;
};
/*
// Copy/Paste
class CopyPaste : public QUndoCommand {
public:
    CopyPaste(DataIOHandler *io, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    DataIOHandler *myIOH;
    string fileName;


};

// Delete Raster
class DeleteRaster : public QUndoCommand {
public:
    DeleteRaster();
    void undo() override;
    void redo() override;

private:


};
// Translation Raster
class TranslationRaster : public QUndoCommand {
public:

    void undo() override;
    void redo() override;

private:


};
// Rotation
class Rotation : public QUndoCommand {
public:

    void undo() override;
    void redo() override;

private:


};
*/
// Create Vector -> Add Point
class CreateVector : public QUndoCommand {
public:
    CreateVector(DataIOHandler *vec, const QPoint &a, const MouseButton &button, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    DataIOHandler *cVec;
    QPoint cP;
    MouseButton cButton;
};

// Delete Vector -> Delete Point
class DeleteVector : public QUndoCommand {
public:
    DeleteVector(DataIOHandler *vec, QPoint a, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    DataIOHandler *dVec;
    QPoint dP;
};

// Translate Vector -> MovePt
class TranslateVector : public QUndoCommand {
public:
    TranslateVector(DataIOHandler *vec, const QPoint &a, size_t index, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    DataIOHandler *tVec;
    QPoint tP;
    size_t tIndex;

};
// Translate Vector Point
class MovePoint : public QUndoCommand {
public:

    MovePoint(SplineVector *vec, QPoint a, size_t index, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SplineVector *myVec;
    QPoint aP;
    size_t myIndex;

};
/*
// Rotate Vector
class RotateVec : public QUndoCommand {
public:

    RotateVec(SplineVector *vec, QPoint a, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    SplineVector *myVec;
    QPoint aP;

};
*/
// Change Vector Width
class ChangeWidth : public QUndoCommand {
public:
    ChangeWidth (DataIOHandler *vec, const int &oldWidth, const int &width, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *wVec;
    int prevWidth;
    int newWidth;
};
// TO-DO : Combine Taper Undo's
// Change Vector Taper
class ChangeVectorTaper1 : public QUndoCommand {
 public:
    ChangeVectorTaper1(DataIOHandler *vec, const int &oldTaper, const int &taper, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *tVec;
    int prevTaper;
    int newTaper;
};
class ChangeVectorTaper2 : public QUndoCommand {
public:
   ChangeVectorTaper2(DataIOHandler *vec, const int &oldTaper, const int &taper, QUndoCommand *parent = nullptr);
   void undo() override;
   void redo() override;
private:
   DataIOHandler *tVec;
   int prevTaper;
   int newTaper;
};
// Copy Vector
class CopyVector : public QUndoCommand {
  public:
    CopyVector(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *cVec;
};
//Paste Vector
class PasteVector : public QUndoCommand {
  public:
    PasteVector(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *pVec;
};
// Change Filter Range
class ChangeFilterRange : public QUndoCommand {
public:
    ChangeFilterRange(DataIOHandler *vec, const int &oldStrength, const int &strength, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *rVec;
    int prevStrength;
    int newStrength;
};
// Insert Layer
class InsertLayer : public QUndoCommand {
public:
    InsertLayer(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *myVec;
};
// Delete Layer
class DeleteLayer : public QUndoCommand {
public:
    DeleteLayer(DataIOHandler *vec ,QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *myVec;
};
// Move Layer Forward
class MoveLayerForward : public QUndoCommand {
public:
    MoveLayerForward(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *mVec;
};
// Move Layer to Front
class MoveLayerToFront : public QUndoCommand {
public:
    MoveLayerToFront(DataIOHandler *vec, const int &oldPos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *myVec;
    int prevPos;
};
// Move Layer Backward
class MoveLayerBackward : public QUndoCommand {
public:
    MoveLayerBackward(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *mVec;
};
// Move Layer to Back
class MoveLayerToBack : public QUndoCommand {
public:
    MoveLayerToBack(DataIOHandler *vec, const int &oldPos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *myVec;
    int prevPos;
};
// Change Layer
class changeActiveLayer : public QUndoCommand {
public:
    changeActiveLayer(DataIOHandler *vec, const int &oldLayer, const int &layer, EditMode mode, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *lVec;
    int prevLayer;
    int newLayer;
    EditMode lMode;
};

#endif // UNDOREDO_H
