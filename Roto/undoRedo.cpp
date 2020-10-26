#include "undoRedo.h"

void UndoRedo::undoAction() {
    undoStack = new QUndoStack(this);
    QAction action = new QAction(tr("&Action"), this);
    undoAction() = undoStack->createUndoAction(this,tr("&Undo"));
    redoAction() = undoStack->createRedoAction(this,tr("&Redo"));
}
UndoRedo::UndoRedo()
{

    undoRedoAction();
}
