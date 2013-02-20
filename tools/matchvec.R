matchvec <- function(X,Y,top=20)
{
  for ( i in 1:top )
  {
    onesOfY = which(Y == 1)
    for ( j in 1:length(onesOfY) )
    {
      if ( any(which(onesOfY[j]==X[i])) )
      {
        print(X[i])
        break;
      }
    }
  }
}

