bool** findequiv(D dfa){
    bool **M;
    M = new bool*[dfa.n];
    for(int i=0;i<dfa.n;i++){
        M[i] = new bool[dfa.n];
        //true for marked
        for(int j=0;j<dfa.n;j++){
            M[i][j] = false;
        }
    }
    int count = 0;
    //Initialization
    for(int i=0;i<dfa.n;i++){
        for(int j=0;j<dfa.n;j++){
            if( (check_final(i,dfa.F) && !check_final(j,dfa.F)) || (!check_final(i,dfa.F) && check_final(j,dfa.F)) ){
                M[i][j] = true;
                count++;
            }
        }
    }
    //Loop
    int countprev = count;
    int countnext = count;
    while(1){
        countprev = count;
        for(int p=0;p<dfa.n;p++){
            for(int q=0;q<dfa.n;q++){
                if(M[p][q]) continue;
                for(int a=0;a<dfa.m;a++){
                    if(M[dfa.del[p][a]][dfa.del[q][a]]){
                        M[p][q] = true;
                        count++;
                    }
                }
            }
        }
        countnext = count;
        if(countnext==countprev) break;
    }
    return M;
}