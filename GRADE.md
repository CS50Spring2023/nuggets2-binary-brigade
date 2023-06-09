# CS50 Functionally correct and complete
## Team of 4: Binary Bridge

### Total (95/100) -- 

## Point Decomposition:

see : https://docs.google.com/spreadsheets/d/1vDDaTogBRajtcILkAtwe-RoFMipuPK3ij3f_TJuDfts/edit?usp=sharing

### Deduction Explanations
#### Design
* -8 design document does not have client modular decomposition and high-level pseudocode 
#### Scrum & Git
* -4 no use of project board

#### Functionality 
* server does not handle invalid seeds (i.e. no error thrown when non-numeric seed given)
* server does not work with our client as player (-2) or as spectator (-1); but works with yours! 
* bad map: 
```
f006gmb@babylon1:~/nuggets-private$ ./checkmap ~/cs50-grading/nuggets-grading/nuggets2-binary-brigade/maps/dungeon.txt 
grid_fromString: string has 1391 chars, but expected 1364 chars for a 31 x 43 grid
```
* tested with your server+client but map has a lot of "dead-space" non-character space where the character is still able to move (-3 overall for map)

* not able to fully test multiple player because of server inability to run with anything but your won client (-2)

#### Penalties
* -2 stdout fills with output (should be putting all that in stderr instead)
* -2 core dumps in coreners of challeg.txt, edges.txt maps