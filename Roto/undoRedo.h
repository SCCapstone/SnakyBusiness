#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QUndoCommand>
#include <QAction>
// sample Undo Functionality

class UndoRedo {
public:
    UndoRedo();
    ~UndoRedo();

    QUndoStack *undoStack = nullptr;
    QAction *undoAction = nullptr;
    QAction *redoAction = nullptr;
    void undoRedoAction();
};

#endif // UNDOREDO_H
