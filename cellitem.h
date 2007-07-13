/*
    Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>

#include "commondefs.h"

class QGraphicsSceneMouseEvent;

/**
 * Graphics item representing single cell on
 * the game field.
 * Handles clicks, emits signals when something important happens :)
 */
class CellItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    CellItem(QGraphicsItem* parent);
    /**
     * Updates item pixmap according to its current
     * state and properties
     */
    void updatePixmap();
    // FIXME: will it EVER be needed to setHasMine(false)???
    /**
     * Sets whether this item holds mine or not
     */
    void setHasMine(bool hasMine) { m_hasMine = hasMine; }
    /**
     * @return whether this item holds mine
     */
    bool hasMine() const { return m_hasMine; }
    /**
     * Sets this item so it holds a digit
     *
     * @param digit digit number (1 to 8)
     */
    void setDigit(int digit) { m_digit = digit; }
    /**
     * @return digit this item holds or 0 if none
     */
    int digit() const { return m_digit; }
    /**
     * Shows what this item hides :)
     * Can be a bomb, a digit, an empty square
     * Note that this method doesn't emit revealed() signal.
     * That signal is only emited as a result of mouse click
     */
    void reveal();
    /**
     * Resets all properties & state of an item to default ones
     */
    void reset();

    // enable use of qgraphicsitem_cast
    enum { Type = UserType + 1 };
    virtual int type() const { return Type; }
signals:
    /**
     * Emitted when this item is revealed with mouse click
     */
    void revealed();
    /**
     * Emitted when flag (not question mark) is set or unset on this item
     *
     * @param isFlagged is true if the flag was set on this item, otherwise - false
     */
    void flaggedStateChanged(bool isFlagged);
private:
    // reimplemented
    virtual void mousePressEvent( QGraphicsSceneMouseEvent * );
    // reimplemented
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * );
    /**
     * Current state of this item
     */
    KMinesState::CellState m_state;
    /**
     * True if this item holds mine
     */
    bool m_hasMine;
    /**
     * True if mine is exploded
     */
    bool m_exploded;
    /**
     * Specifies a digit this item holds. 0 if none
     */
    int m_digit;
};

#endif
