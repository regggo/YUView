#include "playlisttreewidget.h"
#include <QDragEnterEvent>
#include <QUrl>
#include <QMimeData>
#include "playlistitem.h"

#include "mainwindow.h"

PlaylistTreeWidget::PlaylistTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSortingEnabled(true);
}

PlaylistItem* PlaylistTreeWidget::getDropTarget(QPoint pos)
{
    PlaylistItem *pItem = dynamic_cast<PlaylistItem*>(this->itemAt(pos));
    if (pItem != NULL)
    {
        // check if dropped on or below/above pItem
        QRect rc = this->visualItemRect(pItem);
        QRect rcNew = QRect(rc.left(), rc.top() + 1, rc.width(), rc.height() - 2);
        if (!rcNew.contains(pos, true))
        {
            // dropped next to pItem
            pItem = NULL;
        }
    }

    return pItem;
}

void PlaylistTreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
    PlaylistItem* dropTarget = getDropTarget(event->pos());
    QList<QTreeWidgetItem*> draggedItems = selectedItems();
    PlaylistItem* draggedItem = dynamic_cast<PlaylistItem*>(draggedItems[0]);
    if( dropTarget && dropTarget->itemType() == VideoItemType && (dropTarget->childCount() != 0 || draggedItems.count() != 1 || draggedItem->itemType() != StatisticsItemType ))
    {
        // no valid drop
        event->ignore();
        return;
    }

    QTreeWidget::dragMoveEvent(event);
}

// TODO: still not working correctly
void PlaylistTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else    // default behavior
    {
        QTreeWidget::dragEnterEvent(event);
    }
}

void PlaylistTreeWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QStringList fileList;
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QUrl url;
            foreach (url, urls)
            {
                QString fileName = url.toLocalFile();

                fileList.append(fileName);
            }
        }
        event->acceptProposedAction();

        // use our main window to open this file
        MainWindow* mainWindow = (MainWindow*)this->window();
        mainWindow->loadFiles(fileList);
    }
    else
    {
        QTreeWidget::dropEvent(event);
    }
//    {
//        QModelIndex droppedIndex = indexAt( event->pos() );
//        if( !droppedIndex.isValid() )
//            return;

//        QTreeWidget::dropEvent(event);

//        DropIndicatorPosition dp = dropIndicatorPosition();
//        if (dp == QAbstractItemView::BelowItem) {
//            droppedIndex = droppedIndex.sibling(droppedIndex.row() + 1, // adjust the row number
//                                                droppedIndex.column());
//        }
//        selectionModel()->select(droppedIndex, QItemSelectionModel::Select);
//    }
}

Qt::DropActions PlaylistTreeWidget::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}
