#include "status.h"
#include "status.moc"

#include <qpainter.h>
#include <qpixmap.h>
#include <kprinter.h>
#include <qobjectlist.h>
#include <qwhatsthis.h>
#include <qlayout.h>

#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>

#include "dialogs.h"
#include "highscores.h"


Status::Status(QWidget *parent, const char *name)
: QWidget(parent, name)
{
// top layout
	QGridLayout *top = new QGridLayout(this, 2, 5, 10, 10);
    top->setResizeMode(QLayout::Fixed);
    top->setColStretch(1, 1);
    top->setColStretch(3, 1);

// status bar
	// mines left LCD
	left = new LCDNumber(this);
	left->installEventFilter(parent);
	QWhatsThis::add(left, i18n("<qt>Mines left.<br/>"
                               "It turns <font color=\"red\">red</font> "
                               "when you have flagged more cases than "
                               "present mines.</qt>"));
    top->addWidget(left, 0, 0);

	// smiley
	smiley = new Smiley(this);
	connect(smiley, SIGNAL(clicked()), SLOT(smileyClicked()));
	smiley->installEventFilter(parent);
	smiley->setFocusPolicy(QWidget::NoFocus);
	QWhatsThis::add(smiley, i18n("Press to start a new game"));
    top->addWidget(smiley, 0, 2);

	// digital clock LCD
	dg = new DigitalClock(this);
	dg->installEventFilter(parent);
	QWhatsThis::add(dg, i18n("<qt>Time elapsed.<br/>"
                             "It turns <font color=\"blue\">blue</font> "
                             "if it is a highscore "
                             "and <font color=\"red\">red</font> "
                             "if it is the best time.</qt>"));
    top->addWidget(dg, 0, 4);

// mines field
    _fieldContainer = new QWidget(this);
    QGridLayout *g = new QGridLayout(_fieldContainer, 1, 1);
    field = new Field(_fieldContainer);
    g->addWidget(field, 0, 0, AlignCenter);
    connect( field, SIGNAL(changeCase(CaseState, int)),
			 SLOT(changeCase(CaseState, int)) );
	connect( field, SIGNAL(updateStatus(bool)), SLOT(update(bool)) );
	connect( field, SIGNAL(gameLost()), SLOT(gameLost()) );
	connect( field, SIGNAL(startTimer()), dg, SLOT(start()) );
	connect( field, SIGNAL(freezeTimer()), dg, SLOT(freeze()) );
	connect( field, SIGNAL(setMood(Smiley::Mood)),
			 smiley, SLOT(setMood(Smiley::Mood)) );
	connect(field, SIGNAL(gameStateChanged(GameState)),
			SLOT(gameStateChangedSlot(GameState)) );
	QWhatsThis::add(field, i18n("Mines field."));

// resume button
    _resumeContainer = new QWidget(this);
    g = new QGridLayout(_resumeContainer, 1, 1);
    QFont f = font();
    f.setBold(true);
    QPushButton *pb
        = new QPushButton(i18n("Press to resume"), _resumeContainer);
    pb->setFont(f);
    connect(pb, SIGNAL(clicked()), field, SLOT(resume()));
    g->addWidget(pb, 0, 0, AlignCenter);

    _stack = new QWidgetStack(this);
    _stack->addWidget(_fieldContainer);
    _stack->addWidget(_resumeContainer);
    _stack->raiseWidget(_fieldContainer);
    top->addMultiCellWidget(_stack, 1, 1, 0, 4);
}

void Status::initGame()
{
	uncovered = 0;
	uncertain = 0;
	marked    = 0;
	emit gameStateChanged(Stopped);
	update(false);
	smiley->setMood(Smiley::Normal);
	Level::Type type = field->level().type();
	if ( type!=Level::Custom ) {
        Score *fs = highscores().firstScore();
        Score *ls = highscores().lastScore();
        dg->setBestScores( fs->score(), ls->score() );
        delete fs;
        delete ls;
    } else dg->setBestScores(0, 0);

	dg->zero();
}

void Status::smileyClicked()
{
    if ( field->isPaused() ) field->resume();
    else restartGame();
}

void Status::restartGame()
{
	field->restart();
	initGame();
}

void Status::newGame(const Level &level)
{
    if ( level.type()!=Level::Custom ) highscores().setGameType(level.type());
	field->setLevel(level);
	initGame();
    updateGeometry();
}

void Status::changeCase(CaseState cs, int inc)
{
	switch (cs) {
	case Uncovered: uncovered += inc; break;
	case Uncertain: uncertain += inc; break;
	case Marked:    marked    += inc; break;
	default:                          break;
	}
}

void Status::update(bool mine)
{
	QString str;
	const Level &level = field->level();
	int r = level.nbMines() - marked;
	int u = level.width()*level.height()
            - level.nbMines() - uncovered; // cannot be negative
    QColor color = (r<0 && u!=0 ? red : white);
    left->setColor(color);
	left->display(r);

	if ( u==0 && !mine ) _endGame(true); // ends only for wins
}

void Status::_endGame(bool won)
{
    field->showMines();
	field->stop();
	dg->freeze();
	emit gameStateChanged(Stopped);
    smiley->setMood(won ? Smiley::Happy : Smiley::Sad);

    if ( field->level().type()==Level::Custom || !won ) return;
    ExtScore score(dg->score(), field->nbActions());
    highscores().submitScore(true, score, this);
}

void Status::print()
{
	KPrinter prt;
	if ( !prt.setup() ) return;

	// repaint all children widgets
	repaint(false);
	const QObjectList *ol = children();
	QObjectListIt it(*ol);
	QObject *o;
	QWidget *w;
	while ( (o=it.current()) ) {
		++it;
		if ( !o->isWidgetType()) continue;
		w = (QWidget *)o;
		w->repaint(false);
	}

	// write the screen region corresponding to the window
	QPainter p(&prt);
	p.drawPixmap(0, 0, QPixmap::grabWindow(winId()));
}

void Status::gameStateChangedSlot(GameState state)
{
    if ( state==Paused ) _stack->raiseWidget(_resumeContainer);
    else _stack->raiseWidget(_fieldContainer);
    emit gameStateChanged(state);
}
