/*
 * Copyright (c) 2001 Mikhail Kourinny (mkourinny@yahoo.com)
 * Copyright (c) 2002 Nicolas HADACEK  (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __SOLVER_H
#define __SOLVER_H

#include <kdialogbase.h>

#include "bfield.h"


class QLabel;
class KProgress;
class SolverPrivate;

class Solver : public QObject
{
 Q_OBJECT
 public:
    Solver(QObject *parent = 0);
    ~Solver();

    /** A method to advice a point placement */
    KGrid2D::Coord advise(BaseField &field, float &probability);

    /** Solve current mine field */
    void solve(BaseField &field, bool noGuess);

    /** Solve without signals/slot (for test programs) */
    bool solveOneStep(BaseField &field);

 signals:
    void solvingDone(bool success);

 private slots:
    bool solveStep();

 private:
    BaseField     *_field;
    bool           _inOneStep, _noGuess;
    SolverPrivate *d;

    bool initSolve(bool oneStep, bool noGuess);
};

class SolvingRateDialog : public KDialogBase
{
 Q_OBJECT
 public:
    SolvingRateDialog(const BaseField &field, QWidget *parent);

 private slots:
    void step();
    void slotOk();
    void solvingDone(bool success);

 private:
    const BaseField &_refField;
    BaseField        _field;
    Solver           _solver;
    uint             _i, _success;
    QLabel          *_label;
    KProgress       *_progress;

    static const uint NB_STEPS = 200;
};

#endif
