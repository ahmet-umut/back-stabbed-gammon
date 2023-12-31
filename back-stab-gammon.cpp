#include <SDL2john>	//custom SDL2 library 
#include <iostream>
#include <stack>
#include <map>

#include <random>
#define rauni random_device()()	/*random unsigned int*/

using namespace std;

#define drali(a,s,d,f) SDL_RenderDrawLine(renderer,a,s,d,f)
#define color SDL_Color
scolor (color c) {SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,255);}	//sets the drawing color for the renderer
#define sclex 999 /*screen length x*/
#define scley 888 /*screen length y*/

color colors[11] = {{255,255,255},{200,200,200},{222,222,222},{111,138,77},{0,0,0}};	//22	20	2	(10,11,02)2	0
color &bgrcolor=colors[0], &trcolor=colors[1], &bgacolor=colors[2]	//3 and 4 is for the players false and true
;//		background		timer			backgammon

#define animac 76	/*animation step dividor const*/
class anima {public: int x1,x2,y1,y2,radiu;	float step,step0;	bool playe;	//for visual understanding of the hamlas
	anima(int x1, int y1,int x2,int y2,int radiu,bool playe) : x1(x1), x2(x2), y1(y1), y2(y2), radiu(radiu), playe(playe), step(pow(pow(x1-x2,2)+pow(y1-y2,2),.4)/animac)	{step0=step;}
	draw(float dt = 1/60.)	//retruns true when the animation ends
		{	scolor(bgrcolor);	dci(x2,y2,radiu);	scolor(colors[3+playe]);	dci(x2+(x1-x2)*step/step0, y2+(y1-y2)*step/step0,radiu);	drali(x1,y1,x2,y2);		step-=dt;	return step<0;	}
	};
stack<anima> animas;	//animations

struct haml {int from,to; bool iescap=false;};	//iescap: is escape hamla
stack<haml> hamls;	//for selection of a random hamla
stack<bool> table[25];	//backgammon table
int eates[2];	//eaten rocks of players[true|false]

/*
<--			
|false	(gray)
-->
TABLE VIEW	and	GAME MOVEMENT
-->
|true	(black)
<--		
*/

multimap<int,int>keytr;	//key timer

template <class t> stack<t>& operator<<(stack<t>&s, t elemen)	{	s.push(elemen);	return s;	}	//so that we can push multiple elements inside a stack like this:	s<<e1<<e2<<...;
template <class t> t& operator*(stack<t>&s)	{return s.top();}	//top()
template <class t> bool operator<<(stack<t>&recei, stack<t>&sourc)	{	if (sourc.empty())	return false;		recei.push(*sourc), sourc.pop();	return true;	}	//migrate one element
//								receiver			source
#define bgasc 64	/*backgammon scale*/
#define rocrad 8	/*rock radius*/
geteatex(bool playe)	{	return sclex-playe*rocrad;	}	//get the horizontal value of the eaten rocks of the player
geteatey(bool playe, int incre=0)	{	return rocrad*(eates[playe]+incre);	}
getx(int tablei)	//table
	{
	if (tablei<7) return sclex-tablei*bgasc;	else if (tablei<13)	return sclex-(2+tablei)*bgasc;	else if (tablei<19)	return sclex+(tablei-27)*bgasc;	else	return sclex+(tablei-25)*bgasc
	;}
gety(int tablei, int incre=0)	//table indice	increment: how many imaginary rocks to assume under the normal place
	{	if (tablei<13)	return (table[tablei].size() + incre) * rocrad*2;	else	return 4*bgasc - (table[tablei].size() + incre) * 2*rocrad;	}
isput(bool playe, int tablei, int chang = true) {	//can playe put his rock at table[tablei]	chang: change. If set to false, does not make the opponent eaten
	int x1 = getx(tablei), y1 = gety(tablei), x2 = geteatex(!playe), y2 = geteatey(!playe);
	if (table[tablei].size() && *table[tablei] == !playe) {	//on top there is a opponent rock
		table[tablei].pop();	eates[!playe]++;	if (table[tablei].size() && *table[tablei] == !playe)
			{	table[tablei].push(!playe);	eates[!playe]--;	return false;	}	//2 oponent rocks are on the top 

		if (chang)	animas.emplace(x1,y1,x2,y2, rocrad, !playe);
		else {	table[tablei].push(!playe);	eates[!playe]--;	}
		return true;
		;}
	return true;
	;}

class hamla {public:	char hamlt;	int from,dice;	bool playe;
	hamla(){}
	hamla(bool playe, char hamlt, int from, int dice) : hamlt(hamlt), from(from), dice(dice), playe(playe) {}
	hamla(bool playe, int dice) : hamlt('r'), dice(dice), playe(playe) {}

	make()	//assumes that the hamla is makeable
		{stack<bool>sb;		int x1 = getx(from), y1,	to;
		cout<<playe;
		switch (hamlt)
			{
			case 'e':	cout<<" e ";	while(*table[from] != playe)	sb << table[from];	//escape
			case 'm':	//move
				cout<<" m "<<from<<" " <<dice<<endl;
				y1 = gety(from);
				isput(playe, to = from + (playe?-dice:dice));	table[to]<<table[from];
				animas.emplace(x1,y1,getx(to),gety(to), rocrad, playe);	while (table[from]<<sb);
			break;
			case 'r':	cout<<" r " << dice << endl;	isput(playe, to = playe ? 25-dice : dice);	table[to].push(playe);	eates[playe]--;		animas.emplace(geteatex(playe),geteatey(playe),getx(to),gety(to), rocrad, playe);	//recover (put eaten)
			break;
			case 'c':	cout<<" c "<<from<<endl;	table[from].pop();	//collect
			}
		}
	show()
		{stack<bool>sb;		int x1 = getx(from), y1,	to;
		scolor(colors[3+playe]);
		switch (hamlt)
			{
			case 'e':	while(*table[from] != playe)	sb << table[from];	//escape
			case 'm':	//move
				y1 = gety(from);
				drali(x1,y1, getx(to),gety(to,1));	while (table[from]<<sb);
			break;
			case 'r':	drali(geteatex(playe),geteatey(playe,-1),getx(to),gety(to,1));	//recover (put eaten)
			break;
			case 'c':;
			}
		}
	};

class hamlse	//hamla sequence
	{public:	bool playe;	hamla hamlas[4];	int hamlac=0;
	hamlse (bool playe) : playe(playe)	{}
	ahaml (int from, int dice, char hamlt='m') {	hamlas[hamlac] = hamla(playe,hamlt,from,dice);	hamlac++;	}
	hamla& operator[] (int index)	{	return hamlas[index];	} 
	char gett(int index)	{	return hamlas[index].hamlt;	}
	char getdice(int index)	{	return hamlas[index].dice;	}
	make()	{	for (int hamlai=0; hamlai!=hamlac; hamlai++)	hamlas[hamlai].make();	}
	};

stack<hamla>hamlas;
movfr (bool playe, int from, int n)	//move n steps
	{int to;
	if ((to = from+n*(1-2*playe)) > 24 || to<1)	return false;	//...
	if (to==from || to<from != playe || table[from].empty() || *table[from] != playe)	return false;	//the source is not available or the direction is not right
	if (!isput(playe,to,false))	return false;	//destination has at least 2 opponents on top		if only 1 opponent is on top, then that opponent is eaten during isput() call
	hamlas.emplace(playe,'m',from,n);
	return true;
	}

mhaml (bool playe, haml hamla) {stack<bool>sb;	int x1 = getx(hamla.from), y1;	//make hamla
	if (hamla.iescap)	while(*table[hamla.from] != playe)	sb << table[hamla.from];
	y1 = gety(hamla.from);	isput(playe,hamla.to);	table[hamla.to]<<table[hamla.from]; animas.emplace(x1,y1,getx(hamla.to),gety(hamla.to), rocrad, playe);
	while (table[hamla.from]<<sb);
	;}

movfo (bool playe, int dice=0) {	//move forced	takes into consideration of every possible move of that dice and chooses one
	if (!dice)	dice = rauni%6+1;
	hamlas = stack<hamla>();
	hamls = stack<haml>();

	bool endgame=true, toproc,look;	stack<bool>sb;
	for (int i=1; i!=25; i++) {
		if (table[i].size())	toproc=*table[i];	sb = stack<bool>();	look=true;
		while (sb<<table[i])
			if (*sb==playe && (playe && i>6 || !playe && i<19))	//if there are directly uncollectable rocks of the player
				{	if (toproc!=playe && isput(playe, playe? 25-dice : dice, false) && look)	{	look=false;	hamlas.emplace(playe,'e',i,dice);	}	endgame=false;	}
		while(table[i]<<sb);
		;}
	stack<int> si;
	if (endgame) {	//check later
		cout<<"endgame"<<endl;
		for (int i = playe?6:24, j=6; i != (playe?0:18); i--, j--)	{	if (table[i].size() && (si.empty() || j>=dice))	si.push(i);	}	if (si.empty())	return false;
		int hamli = rauni % si.size();	for (;hamli--;)	si.pop();
		if (movfr(playe,*si,dice))	(*hamlas).make();		else	table[*si].pop();
		animas.emplace(1,1,-88,-88,1,1);
		return true;
		}

	for (int i=1; i!=25; i++)	if (table[i].size() && *table[i]==playe && (playe && i>6 || !playe && i<19)) hamls = stack<haml>();

	hamlas = stack<hamla>();
	for (int i = 24; i; i--)	movfr(playe,i,dice);	if (hamlas.empty())	return true;	//if no possible hamlas
	int hamli = rauni % hamlas.size();			for (;hamli--;)	hamlas.pop();		(*hamlas).make();
	return true;
	;}
play (bool playe) {int remai=2, dice1,dice2, x1,y1, dice, tablei;	bool diceu[3] = {0,0,0};	stack<int> dices,si;	//dice is used (diceu_0 is not used)	nanima: no animation
	dice1 = rauni%6+1, dice2 = rauni%6+1;	if (dice1==dice2)	dices<<dice1<<dice1<<dice1<<dice1;	else dices<<dice1<<dice2;
	x1 = geteatex(playe), y1=geteatey(playe);	//copies for animation

	si = stack<int>();
	while (eates[playe] && dices.size())
		{	//finish all eaten rocks of that player if exists
		if (isput(playe, tablei = playe ? 25-*dices : *dices, false))	{	hamla(playe,*dices).make();	dices.pop();	}
		else si<<dices;
		}
	if (eates[playe])	return true;
	while (dices<<si);
	while (dices.size())	{	if (!movfo(playe,*dices)) return false;	dices.pop();	}	//if any remaining hamlas left, play those
	return true;
	}


main (int argv, char** args) {int keboi;
	;SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Back-stabbed-gammon", 100, 100, sclex,scley ,SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_RaiseWindow(window);

	keytr.emplace(pair(SDL_SCANCODE_SPACE,-1));

	for (int rocco=6; rocco--;)	{	char posit = rauni%24+1;		table[posit].push(true);	table[25-posit].push(false);	}

	int trrad=33, tlast=SDL_GetTicks(), hamco = rauni%2;	//trrad: timer radius	tlast = last number of ticks since the last game step	hamco: hamla count (%2 to determine the first player)
	float t=0, dt=1/60., pradt;	//t is the time passed since this line is executed.	dt is the resolution of our "update"s. We could do it faster by reducing dt, or making several updates in a single loop step. My screen is 60 FPS; should it be 144, I'd make dt 1/144 (seconds)	pradt is the practical delta time

	SDL_Event event;	//to not make the program seem like in a not responding state, and to get the keyboard input properly since the SDL_PollEvent fn. takes events such as keyboard events...
	while(1)
	{
		SDL_WaitEventTimeout(&event, dt*333);	if (event.type == SDL_QUIT) break;	//quit properly when desired
		//the timeout is for the game to use much less CPU. Waits for a event (mouse movement, click, etc) for at most dt/3 seconds then continues the process. During the wait time OS can do other things so the CPU usage is low enough.

		if (SDL_GetTicks()-tlast > dt*1000) {pradt = (SDL_GetTicks()-tlast)/1000.;	tlast=SDL_GetTicks();
			scolor(bgrcolor);	SDL_RenderClear(renderer);	//clears the renderer with background color

			//draw the backgammon  sticks	
			scolor(bgacolor);
			drali(sclex,bgasc/8,sclex-bgasc*14,bgasc/8), drali(sclex-bgasc*14,0,sclex-bgasc*14,bgasc*4), drali(sclex-bgasc*14,bgasc*4,sclex-bgasc,bgasc*4);	//table's indicator lines 
			for (auto i=1; i<7; i++) {	
				drali(sclex-i*bgasc,0,sclex-i*bgasc,bgasc), drali(sclex-(8+i)*bgasc,0,sclex-(8+i)*bgasc,bgasc);	//up><
				drali(sclex-i*bgasc,bgasc*3,sclex-i*bgasc,bgasc*4), drali(sclex-(8+i)*bgasc,bgasc*3,sclex-(8+i)*bgasc,bgasc*4);	//down><
				}


			//draw the rocks
			////////////////////////////
			//draw the eaten rocks
			for (int a = eates[0]; a--;)	scolor(colors[3]), dci(sclex,a*rocrad,rocrad);
			for (int a = eates[1]; a--;)	scolor(colors[4]), dci(sclex-rocrad,a*rocrad,rocrad);

			int i=1;
			stack<bool>sb;
			for (;i<7; i++) {	//>up
				sb = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dci(getx(i),gety(i),rocrad);	sb<<table[i];	}
				while (sb.size())	table[i]<<sb;
				;}
			for (;i<13; i++) {	//<up
				sb = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dci(getx(i),gety(i),rocrad);	sb<<table[i];	}
				while (sb.size())	table[i]<<sb;
				;}
			for (;i<19; i++) {	//<down
				sb = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dci(getx(i),gety(i),rocrad);	sb<<table[i];	}
				while (sb.size())	table[i]<<sb;
				;}
			for (;i<25; i++) {	//>down
				sb = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dci(getx(i),gety(i),rocrad);	sb<<table[i];	}
				while (sb.size())	table[i]<<sb;
				;}

			// input handler
			for (auto itera = keytr.begin(); itera != keytr.end(); itera++)	itera->second--;
			bool playe = hamco%2;
			// if (SDL_GetKeyboardState(&keboi)[SDL_SCANCODE_SPACE])	keytr.find(SDL_SCANCODE_SPACE)->second = 22;
			if (keytr.find(SDL_SCANCODE_SPACE)->second < 0 && SDL_GetKeyboardState(&keboi)[SDL_SCANCODE_SPACE])
				{
				keytr.find(SDL_SCANCODE_SPACE)->second = 22;
				if (!play(playe)) break;	//game ends
				hamco++;
				}

			if (animas.size() && (*animas).draw(pradt))	{animas.pop();}


			scolor(trcolor);
			drali(trrad+9,trrad+9,trrad+9+(cos(t)*trrad),trrad+9+(trrad*sin(t)));
			drali(trrad+9,trrad+9,trrad+9+(cos(t+dt*2)*trrad),trrad+9+(trrad*sin(t+dt*2)));
			drali(trrad+9,trrad+9,trrad+9+(cos(t-dt*2)*trrad),trrad+9+(trrad*sin(t-dt*2)));

			SDL_RenderPresent(renderer);	//render the visual output to screen
			t+=pradt;
	}}cout<<"game finished."<<endl; SDL_Quit();return 0;}
