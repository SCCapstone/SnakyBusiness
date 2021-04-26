#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QUndoCommand>
#include <QAction>
#include <dataIOHandler.h>
#include <layer.h>
#include <brushhandler.h>
// sample Undo Functionality

<<<<<<< Updated upstream
// Brush Undo
class BrushUndo : public QUndoCommand {
public:
    BrushUndo(const brushHandler &brush, Layer *layer, const QPoint &qp, QUndoCommand *parent = nullptr);
=======
/*
class Triple {
public:
    Triple(int orientation, QPoint qp, vector<vector<QRgb>> colors);

private:
    int index;
    QPoint point;
    vector<vector<QRgb>> delta;
};

class RadialTriple {
public:
    RadialTriple(QPoint qp, vector<vector<QRgb>> colors);

private:
    QPoint point;
    vector<vector<QRgb>> delta;
};
class LayerChange : public QUndoCommand {
public:
    LayerChange(int from, int to);

private:
    int prevLayer;
    int currLayer;
};

// Brush action -- signal if draw has been started and/or has stopped
class BrushAction : public QUndoCommand {
public:
    BrushAction(bool start, bool stop);
    bool hasStarted(bool start);
    bool hasStopped(bool stop);
private:
    bool started;
    bool stopped;
};

// Brush Undo
/*
class BrushUndo : public QUndoCommand {
public:
    BrushUndo(brushHandler bh, list <Triple> l, QUndoCommand *parent=nullptr);
>>>>>>> Stashed changes
    void undo() override;
    void redo() override;

private:
    brushHandler myBrush;
<<<<<<< Updated upstream
    Layer *myLayer;
    QPoint myPoint;
};
//TO-DO : Implement
=======
    list <Triple> trip;
};
class RadialBrushUndo : public QUndoCommand {
public:
    RadialBrushUndo(brushHandler *bh, QPoint qp, vector<vector<QRgb>> colors, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    brushHandler *myBrush;
    QPoint myPoint;
    vector <vector<QRgb>> deltaColors;
};

>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
    TranslateVector(Layer *vec, const QPoint &pos, QUndoCommand *parent = nullptr);
=======
    TranslateVector(Layer *vec, const QPoint &prevPos, const QPoint &pos, QUndoCommand *parent = nullptr);
>>>>>>> Stashed changes
    void undo() override;
    void redo() override;

private:
    Layer *tVec;
<<<<<<< Updated upstream
=======
    QPoint oldPos;
>>>>>>> Stashed changes
    QPoint newPos;
};
// Rotate Vector
class RotateVec : public QUndoCommand {
public:

    RotateVec(Layer *vec, const QPoint &pos, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Layer *myVec;
    QPoint aP;

};
<<<<<<< Updated upstream
=======
*/
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
// Copy Vector
class CopyVector : public QUndoCommand {
  public:
    CopyVector(DataIOHandler *vec, QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;
private:
    DataIOHandler *cVec;
};
=======
>>>>>>> Stashed changes
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
