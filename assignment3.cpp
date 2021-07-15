#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stdlib.h>
using namespace std;

template <class Container>
void split(const string &str, Container &cont)
{ //file lines parsing from ','
    stringstream ss(str);
    string token;
    while (getline(ss, token, ','))
        cont.push_back(token);
}

struct TeamData
{
    string awayTeam;
    int minute;
    int matchID;
    TeamData *down;
    TeamData *up;
};

struct PlayerData
{
    string playername;
    string teamName;
    PlayerData *next;
    TeamData *down;
};

TeamData *insertdown(TeamData *team, vector<string> words, int min, int id)
{
    TeamData *iter = team;
    if (iter->matchID > id)
    { //datas of doubly linked list passes first node.
        TeamData *temp = new TeamData;
        temp->down = iter;
        temp->down->up = temp;
        temp->up = NULL;
        temp->awayTeam = words[2];
        temp->minute = min;
        temp->matchID = id;
        return temp;
    }
    while (iter->down != NULL && iter->down->matchID <= id)
        //sorted by ids
        iter = iter->down;
    TeamData *temp = new TeamData; 
    temp->down = iter->down;
    iter->down = temp;
    temp->up = iter;
    if (temp->down != NULL)
        temp->down->up = temp;
    temp->awayTeam = words[2];
    temp->matchID = id;
    temp->minute = min;
    return team;
}

PlayerData *insert(PlayerData *player, vector<string> words, int min, int id)
{
    if (player == NULL)
    { //first creation of linked list
        player = new PlayerData;
        player->playername = words[0];
        player->teamName = words[1];
        player->next = NULL;
        player->down = new TeamData;
        player->down->awayTeam = words[2];
        player->down->minute = min;
        player->down->matchID = id;
        player->down->down = NULL;
        player->down->up = NULL;
        return player;
    }
    if (player->playername.compare(words[0]) > 0)
    { //adding head of linked list
        PlayerData *temp = new PlayerData;
        temp->playername = words[0];
        temp->teamName = words[1];
        temp->next = player;
        temp->down = new TeamData;
        temp->down->awayTeam = words[2];
        temp->down->minute = min;
        temp->down->matchID = id;
        temp->down->down = NULL;
        temp->down->up = NULL;
        return temp;
    }
    PlayerData *iter = player;
    while (iter->next != NULL && iter->next->playername <= words[0])
        iter = iter->next;
    if (iter->playername.compare(words[0]) == 0)
    {
        //add down because there are same footballer name
        iter->down = insertdown(iter->down, words, min, id);
        return player;
    }
    PlayerData *temp = new PlayerData; //add next linked list
    temp->next = iter->next;
    iter->next = temp;
    temp->down = new TeamData;
    temp->playername = words[0];
    temp->teamName = words[1];
    temp->down->awayTeam = words[2];
    temp->down->matchID = id;
    temp->down->minute = min;
    temp->down->down = NULL;
    temp->down->up = NULL;
    return player;
}

void mostScoredHalf(int counter0, int counter1,ofstream& out)
{
    out << "1)THE MOST SCORED HALF\n";
    if (counter0 > counter1)
        out << 0;
    else if (counter1 > counter0)
        out << 1;
    else
        out << -1;
    out << endl;
}
void goalScorer(PlayerData *player,ofstream& out)
{
    out << "2)GOAL SCORER\n";
    int maxgoal = 0;
    PlayerData *iter = player;
    PlayerData *iter2 = player;
    int counter = 0;
    while (iter != NULL)
    {
        counter = 0;
        while (iter->down->down != NULL) //go until next down will be null
        {
            counter++;
            iter->down = iter->down->down;
        }
        if (maxgoal < counter)
            maxgoal = counter;
        iter = iter->next;
    }
    iter2 = player;
    while (iter2 != NULL) //back to the first down node
    {
        counter = 0;
        while (iter2->down->up != NULL)
        {
            counter++;
            iter2->down = iter2->down->up;
        }
        if (maxgoal == counter)
            out << iter2->playername << endl;
        iter2 = iter2->next;
    }
}

void hatTrick(PlayerData *player,ofstream& out)
{
    out << "3)THE NAMES OF FOOTBALLERS WHO SCORED HAT-TRICK\n";
    PlayerData *iter = player;
    int written=0;      //If something is written to txt  return 1 else 0
    while (iter != NULL)
    {
        while (iter->down->down != NULL && iter->down->down->down != NULL)
        {
            if (iter->down->matchID == iter->down->down->down->matchID)
            { //control if 1st and 3rd match ids are equal print the name of the footballer
                out << iter->playername << endl;
                written=1;
                while (iter->down->up != NULL)
                    iter->down = iter->down->up;
                if (iter->next != NULL) //iterate until next node will be null
                    iter = iter->next;
                else
                    break;
            }
            else
                iter->down = iter->down->down; //otherwise go next iter.
        }
        while (iter->down->up != NULL)
            iter->down = iter->down->up;
        if (iter->next != NULL)
            iter = iter->next;
        else
            break;
    }
    if (!written)
        out <<"False"<<endl;
}

void teams(PlayerData *player,ofstream& out)
{
    out << "4)LIST OF TEAMS\n";
    int used, i;
    vector<string> unusedTeams;
    PlayerData *iter = player;
    while (iter != NULL)
    {
        used = 0;
        while (iter->down->down != NULL)
        {
            for (i = 0; i < unusedTeams.size(); i++)
                if (iter->down->awayTeam.compare(unusedTeams[i]) == 0)
                    used = 1; //If team name is in unused teams
            if (used == 0)    //if team name is not in unused teams
                unusedTeams.push_back(iter->down->awayTeam);
            iter->down = iter->down->down;
        }
        used = 0;
        for (i = 0; i < unusedTeams.size(); i++) //control last down before null
            if (iter->down->awayTeam.compare(unusedTeams[i]) == 0)
                used = 1;
        if (used == 0)
            unusedTeams.push_back(iter->down->awayTeam);
        while (iter->down->up != NULL) //go up
            iter->down = iter->down->up;
        used = 0;
        for (i = 0; i < unusedTeams.size(); i++) //control lastly the playerteam
            if (iter->teamName.compare(unusedTeams[i]) == 0)
                used = 1;
        if (used == 0)
            unusedTeams.push_back(iter->teamName);
        if (iter->next != NULL) //iterate until next node will be null
            iter = iter->next;
        else
            break;
    }
    for (i = 0; i < unusedTeams.size(); i++)
        out << unusedTeams[i] << endl;
}

void displayPlayers(PlayerData *player,ofstream& out)
{
    out << "5)LIST OF FOOTBALLERS" << endl;
    while (player != NULL)
    {
        out << player->playername << endl;
        player = player->next;
    }
}

void footballerMatches(PlayerData *player, string footballers,ofstream& out)
{
    out << "Matches of " << footballers << endl;
    while (player != NULL)
    {
        if (player->playername.compare(footballers) == 0)
        {
            while (player->down != NULL)
            {
                out << "Footballer Name: " << player->playername;
                out << ",Away Team: " << player->down->awayTeam;
                out << ",Min of Goal: " << player->down->minute;
                out << ",Match ID: " << player->down->matchID << endl;
                if (player->down->down != NULL)
                    player->down = player->down->down;
                else
                    break;
            }
            while (player->down->up != NULL) //go up
                player->down = player->down->up;
        }
        if (player->next != NULL)
            player = player->next;
        else
            break;
    }
}

void ascendingOrder(PlayerData *player, string footballers,ofstream& out)
{
    while (player != NULL)
    {
        if (player->playername.compare(footballers) == 0)
        {
            while (player->down != NULL)
            { //iterate over down nodes
                if (player->down->down != NULL && player->down->matchID != player->down->down->matchID)
                {
                    out << "footballer Name: " << player->playername;
                    out << ",Match ID: " << player->down->matchID << endl;
                }
                else if (player->down->down == NULL)
                { //if down node of current down node is null
                    out << "footballer Name: " << player->playername;
                    out << ",Match ID: " << player->down->matchID << endl;
                }
                if (player->down->down != NULL) //go down
                    player->down = player->down->down;
                else
                    break;
            }
            while (player->down->up != NULL) //go up
                player->down = player->down->up;
        }
        if (player->next != NULL)
            player = player->next;
        else
            break;
    }
}

void descendingOrder(PlayerData *player, string footballers,ofstream& out)
{
    while (player != NULL)
    {
        if (player->playername.compare(footballers) == 0)
        {
            while (player->down->down != NULL) //go down until see null
                player->down = player->down->down;
            while (player->down != NULL)
            { //iterate until up is null
                if (player->down->up != NULL && player->down->matchID != player->down->up->matchID)
                { //if match ids are equal
                    out << "\nfootballer Name: " << player->playername;
                    out << ",Match ID: " << player->down->matchID;
                }
                else if (player->down->up == NULL)
                {
                    out << "\nfootballer Name: " << player->playername;
                    out << ",Match ID: " << player->down->matchID;
                }
                if (player->down->up != NULL)
                    player->down = player->down->up;
                else
                    break;
            }
        }
        if (player->next != NULL)
            player = player->next;
        else
            break;
    }
}

int main(int argc, char *argv[])
{
    PlayerData *player = NULL;
    int counter0 = 0, counter1 = 0, minute, matchid;
    string line;
    ifstream inputFile;
    inputFile.open(argv[1]);
    while (getline(inputFile, line)) //read input line by line
    {
        vector<string> words;
        split(line, words);
        minute = atoi(words[3].c_str());
        matchid = atoi(words[4].c_str());
        if (minute < 46) //1.experiment counters
            counter0++;
        else
            counter1++;
        player = insert(player, words, minute, matchid); //passing linkedlist
    }
    inputFile.close();//input file closed

    ofstream out;
    out.open(argv[3]);

    mostScoredHalf(counter0, counter1,out); //1.
    goalScorer(player,out);                 //2.
    hatTrick(player,out);                   //3.
    teams(player,out);                      //4.
    displayPlayers(player,out);             //5.

    ifstream operationFile;
    operationFile.open(argv[2]);
    int i = 0;
    string line2;
    vector<string> footballers;

    getline(operationFile, line2);
    split(line2, footballers);
    out << "6)MATCHES OF GIVEN FOOTBALLER\n";
    for (i = 0; i < footballers.size(); i++)
        footballerMatches(player, footballers[i],out); //6.

    getline(operationFile, line2);
    footballers.clear();
    split(line2, footballers);
    out << "7)ASCENDING ORDER ACCORDING TO MATCH ID\n";
    for (i = 0; i < footballers.size(); i++)
        ascendingOrder(player, footballers[i],out); //7.

    getline(operationFile, line2);
    footballers.clear();
    split(line2, footballers);
    out << "8)DESCENDING ORDER ACCORDING TO MATCH ID";
    for (i = 0; i < footballers.size(); i++)
        descendingOrder(player, footballers[i],out); //8.
    
    out.close();
    operationFile.close();
    return 0;
}