
// New Esp Manager by Dark Team!
// Credit : Telegram @dark_fack
// Exclusive to everyone who meets in our community

//**********************************************


std::vector<void *> entitys;

bool entityFind(void *ent) {
    if (ent != NULL) {
        for (int i = 0; i < entitys.size(); i++) { //looped//
            if (ent == entitys[i]) return true;
        }
    }
    return false;
}

//---------------------------------------

/*std::vector<void *> entitys1;

bool entityFind1(void *ent1) {
    if (ent1 != NULL) {
        for (int i = 0; i < entitys1.size(); i++) { //looped//
            if (ent1 == entitys1[i]) return true;
        }
    }
    return false;
}*/



/*void clearPlayers() {
    vector<void*> entitys1;
    for (int i = 0; i < entitys1.size(); i++) {
        if (entitys1[i] != NULL) {
            entitys1.push_back(entitys1[i]);
        }
    }
    entitys1 = entitys1;
}*/

/*bool playerFind(void *pl) {
    if (pl != NULL) {
        for (int i = 0; i < players.size(); i++) {
            if (pl == players[i]) return true;
        }
    }
    return false;
}*/



//************************************************

