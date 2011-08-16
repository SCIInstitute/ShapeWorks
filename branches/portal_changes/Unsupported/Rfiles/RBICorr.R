RBIRegressions<-function(P)
{
  rs = 2;   # response start  (e.g. PCA mode projections)
  re = 8;   # response end
  ps = 10;  # predictors start (e.g. clinical scores)
  pe = 17;  # predictors end
  cv = 9;   # covariate position  (e.g. Age)

  vecsz = ((re - rs) +1) * ((pe - ps) +1) + ((re - rs) +1);
  
  r2   <- vector("numeric",vecsz);
  ar2  <- vector("numeric",vecsz);
  f    <- vector("numeric",vecsz);
  fndf <- vector("numeric",vecsz);
  fddf <- vector("numeric",vecsz);
  p    <- vector("numeric",vecsz);
  aic  <- vector("numeric",vecsz);
  call <- vector("character",vecsz);
  sig  <- vector("numeric",vecsz);
  dar2 <- vector("numeric",vecsz);
  daic <- vector("numeric",vecsz);

  q <- 0;
  for (i in rs:re)  # each mode
    {
    q       <- q +1;
    fit     <- lm(P[,i]~P[,cv]);
    sum     <- summary(fit);
    call[q] <- paste(names(P)[i],"~",names(P)[cv]);
    r2[q]   <- sum$r.squared;
    ar2[q]  <- sum$adj.r.squared;
    f[q]    <- sum$fstatistic[1];
    fndf[q] <- sum$fstatistic[2];
    fddf[q] <- sum$fstatistic[3];
    p[q]    <- 1-pf(f[q],fndf[q],fddf[q]);
    if (p[q] < 0.05 ) { sig[q] <- 1; } else { sig[q] <- 0 };
    aic[q]  <- AIC(fit);
    
    dar2[q] <- 0;
    daic[q] <- 0;

    for (j in ps:pe) # each score
      {        
        q       <- q +1;
        fit     <- lm(P[,i]~P[,j]);
        sum     <- summary(fit);
        call[q] <- paste(names(P)[i],"~",names(P)[j]);
        r2[q]   <- sum$r.squared;
        ar2[q]  <- sum$adj.r.squared;
        f[q]    <- sum$fstatistic[1];
        fndf[q] <- sum$fstatistic[2];
        fddf[q] <- sum$fstatistic[3];
        p[q]    <- 1-pf(f[q],fndf[q],fddf[q]);
        if (p[q] < 0.05 ) { sig[q] <- 1; } else { sig[q] <- 0 };
        aic[q]  <- AIC(fit);

        dar2[q] <- 0;
        daic[q] <- 0;
    
        q       <- q+1;
        fit     <- lm(P[,i]~P[,j]+P[,cv]);
        sum     <- summary(fit);
        call[q] <- paste(names(P)[i],"~",names(P)[j],"+",names(P)[cv]);
        r2[q]   <- sum$r.squared;
        ar2[q]  <- sum$adj.r.squared;
        f[q]    <- sum$fstatistic[1];
        fndf[q] <- sum$fstatistic[2];
        fddf[q] <- sum$fstatistic[3];
        p[q]    <- 1-pf(f[q],fndf[q],fddf[q]);
        if (p[q] < 0.05 ) { sig[q] <- 1; } else { sig[q] <- 0 };
        aic[q]  <- AIC(fit);

        dar2[q] <- ar2[q] - ar2[q-1];
        daic[q] <- aic[q] - aic[q-1];

      }
    }

    ans <- data.frame(call=call,p=p,sig=sig,r.squared=r2,adj.r.squared=ar2,
                      change.adj.r.ssquared=dar2,f=f, numdf=fndf, 
                      dendf=fddf,aic=aic,change.aic=daic);
    return (ans);  
}
