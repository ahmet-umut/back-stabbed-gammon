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
#define mumap multimap

color colors[11] = {{255,255,255},{200,200,200},{222,222,222},{111,138,77},{0,0,0}};	//22	20	2	(10,11,02)2	0
color &bgrcolor=colors[0], &trcolor=colors[1], &bgacolor=colors[2]
;//		background		timer			backgammon

#define animac 3
class anima {public: int x1,x2,y1,y2,radiu, step,step0;	bool playe;
	anima(int x1, int y1,int x2,int y2,int radiu,bool playe) : x1(x1), x2(x2), y1(y1), y2(y2), radiu(radiu), playe(playe), step(pow(pow(x1-x2,2)+pow(y1-y2,2),1./animac))	{step0=step;}
	draw() {	scolor(bgrcolor);	dc(x2,y2,radiu);	scolor(colors[3+playe]);	dc(x2+(x1-x2)*step/step0, y2+(y1-y2)*step/step0,radiu);	drali(x1,y1,x2,y2);		return step--<0;	}	//retruns true when the animation ends
	};

stack<int> si;	//intermedieate stack int
stack<anima> sanima;
stack<bool> s;	//intermediate stack
stack<bool> table[25];	//table_0 is for the pop_back process
bool ganima=true;
int eates[2];	//eatens
struct haml {int from,to; bool iescap=false;};	//iescap: is escape hamla
stack<haml> hamls;
/*
<--			
|false	(gray)
-->
TABLE VIEW
-->
|true	(black)
<--		
*/

mumap<int,int>keytr;	//key timer

template <class t> t& operator*(stack<t>&s)	{return s.top();}	//top()
template <class t> bool operator<<(stack<t>&s2, stack<t>&s1)	{	if (s1.empty())	return false;		s2.push(*s1), s1.pop();	return true;	}	//aktarmak

#define bgasc 64	/*backgammon scale*/
#define rocsc 8	/*rock scale*/
geteatex(bool playe)	{	return sclex-playe*rocsc;	}
geteatey(bool playe)	{	return rocsc*eates[playe];	}
getx(int tablei) {
	if (tablei<7) return sclex-tablei*bgasc;	else if (tablei<13)	return sclex-(2+tablei)*bgasc;	else if (tablei<19)	return sclex+(tablei-27)*bgasc;	else	return sclex+(tablei-25)*bgasc
	;}
gety(int tablei)	
	{	if (tablei<13)	return 2*table[tablei].size()*rocsc;	else	return 4*bgasc-2*table[tablei].size()*rocsc;	}
isput(bool playe, int tablei, int chang = true) {	//can playe put his rock at table[tablei]	chang: change. If set to false, does not make the opponent eaten
	int x1 = getx(tablei), y1 = gety(tablei), x2 = geteatex(!playe), y2 = geteatey(!playe);
	if (table[tablei].size() && *table[tablei] == !playe) {	//on top there is a opponent rock
		table[tablei].pop();	eates[!playe]++;	if (table[tablei].size() && *table[tablei] == !playe)
			{	table[tablei].push(!playe);	eates[!playe]--;	return false;	}	//2 oponent rocks are on the top 

		if (chang)	{ sanima.emplace(x1,y1,x2,y2, rocsc, !playe);	}
		else {	table[tablei].push(!playe);	eates[!playe]--;	}
		return true;
		;}
	return true;
	;}

//ohaml = out hamla
movfrto (bool playe, int from, int to)	{
	if (to==from || to<from != playe || table[from].empty() || *table[from] != playe)	return false;	//the source is not available or the direction is not right
	if (!isput(playe,to,false))	return false;	//destination has at least 2 opponents on top	if only 1 opponent is on top, then that opponent is eaten
	haml hamla;	hamla.from = from, hamla.to = to;	hamls.push(hamla);
	return true;
	;}
movfr (bool playe, int from, int n)
	{	if (from+n*(1-2*playe) > 24 || from+n*(1-2*playe) < 1)	return false;	return movfrto(playe,from,from+n*(1-2*playe));	}
movto (bool playe, int to, int n)
	{	if (to-n*(1-2*playe) > 24 || to-n*(1-2*playe) < 1)	return false;	return movfrto(playe,to-n*(1-2*playe),to);	}

mhaml (bool playe, haml hamla) {stack<bool>si;	int x1 = getx(hamla.from), y1;	//make hamla
	if (hamla.iescap)	while(*table[hamla.from] != playe)	si << table[hamla.from];
	y1 = gety(hamla.from);	isput(playe,hamla.to);	table[hamla.to]<<table[hamla.from]; sanima.emplace(x1,y1,getx(hamla.to),gety(hamla.to), rocsc, playe);
	while (table[hamla.from]<<si);
	;}
movfo (bool playe, int dice=0) {	//move forced
	hamls = stack<haml>();	if (!dice)	dice = rauni%6+1;

	bool endgame=true, toproc,look;		
	for (int i=1; i!=25; i++) {
		if (table[i].size())	toproc=*table[i];	s = stack<bool>();	look=true;
		while (s<<table[i])
			if (*s==playe && (playe && i>6 || !playe && i<19))
				{	if (toproc!=playe && isput(playe, playe? 25-dice : dice, false) && look)	{	look=false;	hamls.push({i, playe? 25-dice : dice, true});	}	endgame=false;	}
		while(table[i]<<s);
		;}
	stack<int> si;
	if (endgame) {
		for (int i = playe?6:24, j=6; i != (playe?0:18); i--, j--)	{	if (table[i].size() && (si.empty() || j>=dice))	si.push(i);	}	if (si.empty())	return false;
		int hamli = rauni % si.size();	for (;hamli--;)	si.pop();
		if (movfr(playe,*si,dice))	mhaml(playe,*hamls);	else	table[*si].pop();
		sanima.emplace(1,1,99,99,1,1);
		return true;
		}

	for (int i=1; i!=25; i++)	if (table[i].size() && *table[i]==playe && (playe && i>6 || !playe && i<19)) hamls = stack<haml>();

	for (int i = 24; i; i--)	movfr(playe,i,dice);	if (hamls.empty())	return true;
	int hamli = rauni % hamls.size();	for (;hamli--;)	hamls.pop();	mhaml(playe, *hamls);	//what if no hamlas??????
	return true;
	;}
play (bool playe) {int remai=2, dice1,dice2, x1,y1, dice, tablei;	bool diceu[3] = {0,0,0};	stack<int> dices,si;	//dice is used (diceu_0 is not used)	nanima: no animation
	dice1 = rauni%6+1, dice2 = rauni%6+1;	if (dice1==dice2)	dices.push(dice1),dices.push(dice1),dices.push(dice1),dices.push(dice1);	else dices.push(dice1),dices.push(dice2);
	x1 = geteatex(playe), y1=geteatey(playe);	//copies for animation

	si = stack<int>();
	while (eates[playe] && dices.size()) {	//finish all eaten rocks of that player if exists
		if (isput(playe, tablei = playe ? 25-*dices : *dices)) {
			dices.pop();	table[tablei].push(playe);	eates[playe]--;		sanima.emplace(x1,y1,getx(tablei),gety(tablei), rocsc,playe);
			}
		else	si<<dices;
		}
	if (eates[playe])	return true;
	while (dices<<si);
	while (dices.size())	{	if (!movfo(playe,*dices)) return false;	dices.pop();	}	//if any remaining hamlas left, play those
	return true;
	}


main (int argv, char** args) {int keboi;
	;SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("SDL2 app", 100, 100, sclex,scley ,SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_RaiseWindow(window);

	keytr.emplace(pair(SDL_SCANCODE_SPACE,-1));

	for (int rocco=3; rocco--;)	{	char posit = rauni%2+12;		table[posit].push(true);	table[25-posit].push(false);	}

	int trrad=33, tlast=SDL_GetTicks();	//tir: timer radius
	float t=0, dt=1/60.;	//t is the time passed since this line is executed.	dt is the resolution of our "update"s. We could do it faster by reducing dt, or making several updates in a single loop step. My screen is 60 FPS; should it be 144, I'd make dt 1/144 (seconds)

	int hamco=rauni%2;	//determine the first player
	bool playui=true;	//play until interrupt

	SDL_Event event;	//to not make the program seem like in a not responding state, and to get the keyboard input properly since the SDL_PollEvent fn. takes events such as keyboard events...
	while(1)
	{
		SDL_PollEvent(&event); if(event.type == SDL_QUIT) break;	//quit properly when desired
		if (SDL_GetTicks()-tlast > dt*1000) {tlast=SDL_GetTicks();
			scolor(bgrcolor);	SDL_RenderClear(renderer);	//clears the renderer with background color
			scolor(bgacolor);
			
			//draw the backgammon  sticks	
			drali(sclex,bgasc/8,sclex-bgasc*14,bgasc/8), drali(sclex-bgasc*14,0,sclex-bgasc*14,bgasc*4), drali(sclex-bgasc*14,bgasc*4,sclex-bgasc,bgasc*4);	//table's indicator lines 
			for (auto i=1; i<7; i++) {	
				drali(sclex-i*bgasc,0,sclex-i*bgasc,bgasc), drali(sclex-(8+i)*bgasc,0,sclex-(8+i)*bgasc,bgasc);	//up><
				drali(sclex-i*bgasc,bgasc*3,sclex-i*bgasc,bgasc*4), drali(sclex-(8+i)*bgasc,bgasc*3,sclex-(8+i)*bgasc,bgasc*4);	//down><
				}


			//draw the rocks
			////////////////////////////
			//draw the eaten rocks
			for (int a = eates[0]; a--;)	scolor(colors[3]), dc(sclex,a*rocsc,rocsc);
			for (int a = eates[1]; a--;)	scolor(colors[4]), dc(sclex-rocsc,a*rocsc,rocsc);

			int i=1;
			for (;i<7; i++) {	//>up
				s = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dc(getx(i),gety(i),rocsc);	s<<table[i];	}
				while (s.size())	table[i]<<s;
				;}
			for (;i<13; i++) {	//<up
				s = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dc(getx(i),gety(i),rocsc);	s<<table[i];	}
				while (s.size())	table[i]<<s;
				;}
			for (;i<19; i++) {	//<down
				s = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dc(getx(i),gety(i),rocsc);	s<<table[i];	}
				while (s.size())	table[i]<<s;
				;}
			for (;i<25; i++) {	//>down
				s = stack<bool>();
				while (table[i].size())	{	scolor(colors[3+*table[i]]);	dc(getx(i),gety(i),rocsc);	s<<table[i];	}
				while (s.size())	table[i]<<s;
				;}

			//input handler
			bool playe = hamco%2;
			for (auto itera = keytr.begin(); itera != keytr.end(); itera++)	itera->second--;
			if (SDL_GetKeyboardState(&keboi)[SDL_SCANCODE_SPACE] && keytr.find(SDL_SCANCODE_SPACE)->second<0)	{
				keytr.find(SDL_SCANCODE_SPACE)->second = 22;
				if (!play(playe))break;
				hamco++;}

			if (!sanima.empty() && (*sanima).draw())	{sanima.pop();}


			scolor(trcolor);
			drali(trrad+9,trrad+9,trrad+9+(cos(t)*trrad),trrad+9+(trrad*sin(t)));
			drali(trrad+9,trrad+9,trrad+9+(cos(t+dt*2)*trrad),trrad+9+(trrad*sin(t+dt*2)));
			drali(trrad+9,trrad+9,trrad+9+(cos(t-dt*2)*trrad),trrad+9+(trrad*sin(t-dt*2)));

			SDL_RenderPresent(renderer);	//render the visual output to screen
			t+=dt;
	}}cout<<"game finished."<<endl; SDL_Quit();return 0;}
