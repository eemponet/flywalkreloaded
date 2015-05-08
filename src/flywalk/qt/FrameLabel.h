/*#    This file is part of Flywalk Reloaded. <http://flywalk.eempo.net>
#
#     flywalk@eempo.net - Pedro Gouveia
#
#
#    Flywalk Reloaded is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 2.
#
#    Flywalk Reloaded is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Flywalk Reloaded.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FRAMELABEL_H
#define FRAMELABEL_H

// Qt
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QLabel>
#include <QMenu>
// Local
#include "Structures.h"

#define SELECT_ROI 0
#define SELECT_LINES 1

class FrameLabel : public QLabel
{
    Q_OBJECT

    public:
        FrameLabel(QWidget *parent = 0);
        void setMouseCursorPos(QPoint);
        QPoint getMouseCursorPos();
        QMenu *menu;
        int selectionTool; 
        
        void updatePixmap(const QPixmap &image);
    private:
        void createContextMenu();
        MouseData mouseData;
        QPoint startPoint;
        QPoint mouseCursorPos;
        bool drawBox;
        QRect *box;

    protected:
        void mouseMoveEvent(QMouseEvent *ev);
        void mousePressEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void paintEvent(QPaintEvent *ev);
        void handleContextMenuAction(QAction *action);
    signals:
        void newMouseData(struct MouseData mouseData);
        void onMouseMoveEvent();
};

#endif // FRAMELABEL_H
