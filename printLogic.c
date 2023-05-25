Psuedocode for grid display:

for each point in grid
  if player
    if visible
      print player
  else (not player)
    if gold
      if visible
        print gold
      else if known
        print "."
      else
        print " "
    else (not gold)
      if known
        print terrain
      else
        print " "

