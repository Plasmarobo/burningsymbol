#ifndef _DIALOG_H_
#define _DIALOG_H_
#include "Entities.h"
#include "State.h"
#include "Map.h"
#include "Data.h"
#include "OrderedPool.h"


State *StartDialog(std::string dscript, std::vector<State*> *stack); //Adds a dialog to the stack




#endif