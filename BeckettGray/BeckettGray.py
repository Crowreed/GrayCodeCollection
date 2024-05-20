import copy
import timeit
import math
import cProfile
import time

def Binary(x,n):
    '''Renvoie la représentation binaire de x sur n bits'''
    b=bin(x)[2:]#On retire l'en-tête
    res=[int(i) for i in b]
    res=[0]*(n-len(res))+res#On rajoute les zéros manquants
    return res




def pw2(n):
    '''Renvoie si n est une puissance de 2'''
    return n!=0 and (n&(n-1))==0



def listepower2(n):
    '''la liste des puissances de 2'''
    res=[]
    for i in range(2**n):
        if pw2(i):
            res.append(i)
    return res



def listeBinaire(n):
    '''Liste de entiers entre 0 et 2^n en binaire'''
    res=[]
    for i in range(2**n):
        res.append(Binary(i,n))
    return res

def listeFlip(n):
    res=[]
    for i in range(2**n):
        res.append([])
        for j in range(n):
            res[i].append(i^(1<<j))
    return res

def listeUn1(n):
    res=[]
    for i in range(2**n):
        if bin(i).count('1')==1:
            res.append(True)
        else:
            res.append(False)
    return res




def BGCHeuristique1(s,x,n,bgc,avail,old,oldS,listev,p2,bi,n2,flip,l11):
    global compteur
    compteur+=1
    if s>=n2:
        listeCodeBGC.append(bgc.copy())
        return
    if (s>1 and s-oldS[0]>nbMax[n]): #Ca fait perdre du temps contrairement au C++
        return
    if l11[x]:
        bool=False
        for i in p2:
            if avail[i]:
                bool=True
                break
        if not bool:
            return
    cmpt=0
    xn=0
    b=0
    for i in range(n):
        fxi=flip[x][i]
        if listev[fxi]==1 and avail[fxi]:
            cmpt+=1
            if cmpt>1:
                return
            xn=fxi
            bit=n-i-1
    if cmpt==1 and (bi[xn][bit]==1 or bit==(old[0] if old else 0)):
        nold = old.copy()
        noldS = oldS.copy()
        if bi[xn][bit] == 1:
            nold.append(bit)
            noldS.append(s)
        else:
            nold.pop(0)
            noldS.pop(0)
        avail[xn]=False
        bgc.append(xn)
        res = listev.copy()
        for j in range(n):
            res[flip[xn][j]]-=1
        BGCHeuristique1(s+1,xn,n,bgc,avail,nold,noldS,res,p2,bi,n2,flip,l11)
        avail[xn]=True
        bgc.pop()
        return
    if cmpt==0:
        for i in range(n):
            xn=flip[x][i]
            ni=n-i-1
            if avail[xn] and (bi[xn][ni]==1 or ni==(old[0] if old else 0)):
                nold = old.copy()
                noldS= oldS.copy()
                if bi[xn][ni] == 1:
                    nold.append(ni)
                    noldS.append(s)
                else:
                    nold.pop(0)
                    noldS.pop(0)
                avail[xn]=False
                bgc.append(xn)
                res = listev.copy()
                for j in range(n):
                    res[flip[xn][j]]-=1
                BGCHeuristique1(s+1,xn,n,bgc,avail,nold,noldS,res,p2,bi,n2,flip,l11)
                avail[xn]=True
                bgc.pop()
        return
    else:
        return
    return

def plusieurs1(m):
    for i in range(m):
        BGCHeuristique1(1,0,n,bgc,avail,[],lv,p22,bi,n2,F)

compteur=0
n=5
n2=2**n
bgc=[0]
listeCodeBGC=[]
avail=[True for i in range(2**n)]
avail[0]=False
lv=[n for i in range(2**n)]
p22=listepower2(n)
bi=listeBinaire(n)
F=listeFlip(n)
nbMax=[0,1,2,8,16,7,10,15,13];
listeNbSommet=[math.comb(n, k) for k in range(0,n+1)]
l11=listeUn1(n)
#t=time.time()
print(timeit.timeit('BGCHeuristique1(1,0,n,bgc,avail,[],[],lv,p22,bi,n2,F,l11)',globals=globals(),number=1))
#cProfile.run('BGCHeuristique1(1,0,n,bgc,avail,[],lv,p22,bi,n2,F)')
#BGCHeuristique1(1,0,n,bgc,avail,[],lv,p22,bi,n2,F)
print(len(listeCodeBGC))
print(compteur)