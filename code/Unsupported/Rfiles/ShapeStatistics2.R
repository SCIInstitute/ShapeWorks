mygauss<-function(sigma,d,rho)
{
# g <- (1 / (sigma * sqrt(2 * pi))) * exp( - (d * d) / (2 * sigma * sigma));
  g <- rho / (rho-1 + exp(-(d*d)/(2 * sigma * sigma)));
  if (d < 0) g <- 1/ g;
  return (g);
}

testgauss<-function(sigma,s,e,rho)
{ 
  d <- s;
  for (i in 1:200)
  {  
   y <- mygauss(sigma,d,rho);

   if (i==1) 
   {
     xlist <- d;
     ylist <- y;
   }
   else 
   {
     xlist<-c(xlist,d);
     ylist<-c(ylist,y);
   }

   d <- d + (e-s) / 200;

  }
  return (data.frame(x=xlist, y=ylist));
}

AIC<-function(P,s,e,numModes)
{
# Compute eigenvalues.  Input is assumed to be projections to
# the first "numFactors" principal axes.
 tol  <- 1.0e-6;
 data <- as.matrix(P[,s:e]);
 xbar <- apply(data,2,mean);
 C <- cov(data - xbar);
 Evals <- as.vector(eigen(C)$values);

 



 return (Evals);
}


MahalanobisTest<-function(samples,dims)
{

  ## Not complete jc 6/2
  data<-GaussianMatrix(samples,dims);

  # Introduce random variances
  vars<-sort(runif(dims,0,100));

  for (i in 1:dims)
  {
  }

 data <- data / (vars * vars)

  C<-cov(data);
  W<-eigen(C);

  RET<-list(vars=vars,evals=W$values);
  return(RET);
}

GaussianMatrix<-function(r,c) 
{ 
 rnds <- rnorm(r*c);
 Q <- matrix(rnds,r,c);
 return(Q);
}

LikelihoodsOfSimulatedData<-function(P,gl,s,e)
{ ## Not Complete 6/2/ jc
  for(i in 1:1)
  {
  S<-GaussianSimulations(P,gl,s,e,15,10,0);  
  
  if (i == 1)  
  {  
    L <- as.vector(S$L[1:25]);
    RL <- as.vector(S$RL[1:25]);
  }  
  else
  {  
    L <- c(L,as.vector(S$L[1:25]));
  }
  }

#  hist(L,plot=TRUE);
#  windows();
#  hist(RL,plot=TRUE);
#  return(L,S$xbar1,S$xbar2,S$v1,S$v2,S$L,S$group1,S$group2);
  return(S);
}


BatchSimulations<-function()
{
  S<-read.table("Struct4.modelengthsnoscale.params.csv",header=TRUE,sep=",");
  RunABunchOfSimulations(S,1,4,28,1000,"S4A");
  S<-read.table("Struct7.modelengthsnoscale.params.csv",header=TRUE,sep=",");
  RunABunchOfSimulations(S,1,4,28,1000,"S7A");
  S<-read.table("Struct0.modelengthsnoscale.params.csv",header=TRUE,sep=",");
  RunABunchOfSimulations(S,1,4,28,1000,"S0A");
}


RunABunchOfSimulations<-function(P,gl,s,e,M,TITLE)
{

## NOT USING THIS ONE ANYMORE  jc- 6/3
   PlotPermutationTests(P,gl,s,e-2,M,paste(TITLE,"REAL",sep=""));
  
   for (i in 1:2)
   {
    S<-GaussianSimulations(P,gl,s,e,15,10,0);
   PlotPermutationTests(S,gl,2,24,M,paste(TITLE,"SIM",toString(i),sep=""));
   PlotPermutationTests(S,gl,27,49,M,paste(TITLE,"JOINTSIM",toString(i),sep=""));
   }
}


#NullHypothesisTests<-function(P,gl,s,e,dims)
#{
#  # Simulate random data from distribution
#  for (i in 1:100)
#  {
#    S<-GaussianSimulations(P,gl,s,e,15,10,0);
#    p <- HotellingT2(S,1,27,27+dims-1);
#    if (i==1) {plist<-p;}
#    else {plist<-c(plist,p);}
#  }

#  return(plist);  
#}


HotellingT2Profile<-function(P,a,s,e,ParallelDims,TXT="HotellingT2Profile")
{
  for (i in s:e) 
  {
   res <- HotellingT2(P,a,s,i);
   if (i==s) t2<-res$t2 else t2<-c(t2,res$t2)
   if (i==s) p<-res$p   else p<-c(p,res$p)
  }

   par<-HDLSSParallelAnalysis(P,s,e,ParallelDims,100);

   #open pdf for output
   pdf(paste(TXT,".pdf",sep=""));

   # Plot all of the real data distribution p-values
   plot(p,type="b",main=paste(TXT,": P"),ylab="P Value",xlab="Number PCA Modes",pch=0);
   legend("topright",c("Param HT2"),col=c("black"), pch=c(0));
   grid();
   plot(t2[1:20],type="b",main=paste(TXT,": T2"),ylab="T2 Value",xlab="Number PCA Modes",pch=0);

   # Plot parallel analysis 
   plot(par$values,type="b", ylab="Percent Variance", xlab="Number of PCA modes", 
        main=paste(TXT, ": Parallel Analysis"));
   lines(par$simulated,type="b",lty=2);
   dev.off();

#   res<-data.frame(T2perm.p=T2perm.p, LDperm.p= LDperm.p, 
#                  T2param.p = T2param.p, PAdiff = par$difference);
   res<-data.frame(p=p, t2 = t2, PAdiff = par$difference);

  write.table(res,paste(TXT,".csv",sep=""),append=FALSE,quote=TRUE,sep=",",col.names=TRUE);
  return(res)
}

HotellingT2<-function(P,grpIDX,datastart,dataend)
{
  groupIDs<-P[,grpIDX];

  #HOTELLING T2 TEST
  group1<-as.matrix(P[groupIDs==1,datastart:dataend])
  group2<-as.matrix(P[groupIDs==2,datastart:dataend]);
  n1<-length(group1[,1]);
  n2<-length(group2[,1]);

  # Find the means and covariances
  xbar1<-apply(group1,2,mean);
  xbar2<-apply(group2,2,mean);
  C1<-cov(group1);
  C2<-cov(group2);

  # Find the pooled covariance matrix and its inverse
  C <- ((n1-1)*C1+(n2-1)*C2)/(n1+n2-2);
  Cinv <- solve(C);

  # Multiply to find the T-square
   p <- length(group1[1,])
  T <- (n1*n2)*t(xbar1-xbar2)%*%Cinv%*%(xbar1-xbar2)/(n1+n2);
  F <- (n1+n2-p-1)*T/((n1+n2-2)*p);
  pval <- 1 - pf(F,df1=p,df2=n1+n2-p-1);

  res<-list(p = pval, t2 = T);
  return(res);
}

PlotPermutationTests<-function(P,a,s,e,M,TXT) 
{

## NOT USING THIS ONE ANYMORE  jc- 6/3
## Not clear the simulation code is correct?  


  for (i in s:e) {
   res<-PermutationTests(P,a,s,i,M)
   if (i==s) T2perm.p<-res$T2perm.p   else T2perm.p<-c(T2perm.p,res$T2perm.p)
  # if (i==s) LDperm.p<-res$LDperm.p   else LDperm.p<-c(LDperm.p,res$LDperm.p)
   if (i==s) T2param.p<-res$T2param.p else T2param.p<-c(T2param.p,res$T2param.p)
  }

   par<-HDLSSParallelAnalysis(P,s,e,3000,100);

   #open pdf for output
   pdf(paste(TXT,".pdf",sep=""));

   # Plot all of the real data distribution p-values
   plot(T2perm.p,type="b",main=TXT,ylab="P Value",xlab="Number PCA Modes",pch=0);
   lines(T2param.p, type="b",col="red",pch=1);
 #  lines(LDperm.p,type="b",col="blue",pch=2);
   legend("topright",c("Perm HT2","Param HT2","LDA"),col=c("black","red","blue"),
          pch=c(0,1,2));
   grid();
   plot(T2perm.p,type="b", main=paste(TXT, ": Permutation Hotelling T2"));
   plot(T2param.p,type="b", main=paste(TXT, ": Parameteric Hotelling T2"));
  # plot(LDperm.p,type="b", main=paste(TXT, ": LDA Permutation"));

   # Plot parallel analysis 
   plot(par$values,type="b", ylab="Percent Variance", xlab="Number of PCA modes", 
        main=paste(TXT, ": Parallel Analysis"));
   lines(par$simulated,type="b",lty=2);
   dev.off();

#   res<-data.frame(T2perm.p=T2perm.p, LDperm.p= LDperm.p, 
#                  T2param.p = T2param.p, PAdiff = par$difference);
   res<-data.frame(T2perm.p=T2perm.p, T2param.p = T2param.p, PAdiff = par$difference);

  write.table(res,paste(TXT,".csv",sep=""),append=FALSE,quote=TRUE,sep=",",col.names=TRUE);
  return(res)
}

PermutationTests<-function(P,grpIDX,datastart,dataend,M) 
{
#  LDcount <- 0;
  T2count <- 0;
  for (i in 1:M) 
   {
    #PERMUTE THE GROUPS
    if (i==1) 
    {
      groupIDs<-P[,grpIDX];
    } 
    else
    {
      groupIDs<-sample(groupIDs);
    }

    # FISHER LDA WITH TEST
#    ld<-FisherLD(P,groupIDs,datastart,dataend);
#    g <- t.test(ld$proj~groupIDs,var.equal=FALSE);
#    t <- as.numeric(g$statistic);

    #HOTELLING T2 TEST
    group1<-as.matrix(P[groupIDs==1,datastart:dataend])
    group2<-as.matrix(P[groupIDs==2,datastart:dataend]);
    n1<-length(group1[,1]);
    n2<-length(group2[,1]);

    # Find the means and covariances
    xbar1<-apply(group1,2,mean);
    xbar2<-apply(group2,2,mean);
    C1<-cov(group1);
    C2<-cov(group2);

    # Find the pooled covariance matrix and its inverse
    C <- ((n1-1)*C1+(n2-1)*C2)/(n1+n2-2);
    Cinv <- solve(C);

    # Multiply to find the T-square
    T <- (n1*n2)*t(xbar1-xbar2)%*%Cinv%*%(xbar1-xbar2)/(n1+n2);
   
    if (i==1)
    {
#      w<-ld$w;
#      pparam<-as.numeric(g$p.value);
#      t.orig <- t;
      T2.orig <- T;
      p <- length(group1[1,])
      F <- (n1+n2-p-1)*T/((n1+n2-2)*p)
      T2param.p <- 1 - pf(F,df1=p,df2=n1+n2-p-1)
      # tlist  <- t;
      # T2list <- T;
    }
    else
    {
#     if (t >= t.orig)  { LDcount <- LDcount +1 };
     if (T >= T2.orig) { T2count <- T2count + 1};
     # tlist <- c(tlist,t);
    }
  }

  T2perm.p <- T2count / (M+1);
#  LDperm.p <- LDcount/(M+1);

 # RES <- list(T2perm.p = T2perm.p, T2param.p = T2param.p, LDperm.p = LDperm.p, discriminant=w);
 RES <- list(T2perm.p = T2perm.p, T2param.p = T2param.p);
  return(RES);
}

FisherLD<-function(P,groupIDs,datastart,dataend) {
# Computes the Fisher linear discriminant (normal) and 
# projection of data onto that line.

  # Compute covariance matrix for each group
  group1<-as.matrix(P[groupIDs==1,datastart:dataend])
  group2<-as.matrix(P[groupIDs==2,datastart:dataend]);
  n1<-length(group1[,1]);
  n2<-length(group2[,1]);

  # Find the means and covariances
  xbar1<-apply(group1,2,mean);
  xbar2<-apply(group2,2,mean);
  C1<-cov(group1);
  C2<-cov(group2);

  # Compute Fisher's linear discriminant
  Cinv <- solve(C1 + C2);
  w <- as.vector(Cinv %*% (xbar1-xbar2));
  w <- w / sqrt(w%*%w);
  
  # Project all samples onto the LD
  i1<-1;
  i2<-1;
  for (i in 1:(n1+n2))
  {
    if (groupIDs[i]==1) { 
      q <- as.vector(group1[i1,]);
      i1 <- i1 + 1;
    } else { 
      q <-as.vector(group2[i2,]);
      i2 <- i2 + 1;
    }
    b <- w %*% q;
    if (i==1) p<-b else  p<-c(p,b);
  }

  pval= t.test(p~groupIDs);
  # Return the linear discriminant and the projected samples
  RES<-list(group=groupIDs,proj=p,w=w,xbar1=xbar1,xbar2=xbar2,Cinv=Cinv,p.value=pval);
  return(RES);
}

HDLSSParallelAnalysis<-function(P,s,e,DIMS,M)
{
 # Does a parallel analysis using the dual space.  The columns of the input data 
 # is assumed to contain the number of factors and the matrix is assumed to have
 # the correct eigenvalues (this is true, for example, for projections of a HDLSS
 # dataset onto all of its nonzero principal eigenvectors).  The number of dimensions
 # of the original dataset must be specified (DIMS).  M is the number of simulations.

 # Extract the data and determine the parameters for the simulation.  Also compute
 # eigenvalues of the covariance of the data.  Normalize eigenvalues wrt total variance.
 tol <- 1.0e-6;
 data<-as.matrix(P[,s:e]);
 C <- cov(data);
 Cvals <- as.vector(eigen(C)$values);
 numFactors <- length(Cvals);
 for (i in 1:numFactors) if (Cvals[i] < tol) Cvals[i] <- 0.0;
 Cvals <- Cvals / sum(Cvals);

 # Run M Monte Carlo simulations of Gaussian distributed noise and 
 # average the sorted eigenvalues.  DIMS samples of numFactor dimensions 
 # gives same eigenvalues as numFactors samples of DIMS dimensions.
 xbar <- vector("numeric",numFactors) + 1.0;
 for (i in 1:M)
 {
   S <-GaussianMatrix(DIMS,numFactors);
   vals <- as.vector((eigen(cov(S)))$values);
   if (i==1) { M<-rbind(t(vals))} else M<-rbind(M,t(vals));
 }
 Svals <- apply(M,2,mean);
 Svals <- Svals / sum(Svals);

 diff <- Cvals-Svals;

# windows();
# plot(Cvals,type="b", main="Scree plot");
# lines(Svals, type="b");

 RET=list(values=Cvals,simulated=Svals,difference=diff);
 return(RET);
}

HDLSSParallelAnalysis2<-function(nf,DIMS,M)
{
 # Does a parallel analysis using the dual space.  The columns of the input data 
 # is assumed to contain the number of factors and the matrix is assumed to have
 # the correct eigenvalues (this is true, for example, for projections of a HDLSS
 # dataset onto all of its nonzero principal eigenvectors).  The number of dimensions
 # of the original dataset must be specified (DIMS).  M is the number of simulations.

 # Extract the data and determine the parameters for the simulation.  Also compute
 # eigenvalues of the covariance of the data.  Normalize eigenvalues wrt total variance.
# tol <- 1.0e-6;
# data<-as.matrix(P[,s:e]);
# C <- cov(data);
# Cvals <- as.vector(eigen(C)$values);
# numFactors <- length(Cvals);
# for (i in 1:numFactors) if (Cvals[i] < tol) Cvals[i] <- 0.0;
# Cvals <- Cvals / sum(Cvals);

numFactors <- nf;

 # Run M Monte Carlo simulations of Gaussian distributed noise and 
 # average the sorted eigenvalues.  DIMS samples of numFactor dimensions 
 # gives same eigenvalues as numFactors samples of DIMS dimensions.
 xbar <- vector("numeric",numFactors) + 1.0;
 for (i in 1:M)
 {
   S <-GaussianMatrix(DIMS,numFactors);
   vals <- as.vector((eigen(cov(S)))$values);
   if (i==1) { M<-rbind(t(vals))} else M<-rbind(M,t(vals));
 }
 Svals <- apply(M,2,mean);
 Svals <- Svals / sum(Svals);

# diff <- Cvals-Svals;

# windows();
# plot(Cvals,type="b", main="Scree plot");
# lines(Svals, type="b");

 RET=list(simulated=Svals);
 return(RET);
}

GaussianSimulations<-function(P, grpIDX, datastart, dataend, numA, numB, noiselevel) {
#                                                
# Generates Gaussian random samples in a high dimensional space in the 
# distributions described by given data P.
#

# Extract the two real groups as matrices.
numDims <- (dataend - datastart)+1;
groupIDs<-P[,grpIDX];
group1<-as.matrix(P[groupIDs==1,datastart:dataend]);
group2<-as.matrix(P[groupIDs==2,datastart:dataend]);
n1<-length(group1[,1]);
n2<-length(group2[,1]);

groupJoint<-as.matrix(P[,datastart:dataend]);

#if (extramodes != 0)
#{
#  var1 <- var(group1[,(dataend-datastart)+1]);
#  var2 <- var(group2[,(dataend-datastart)+1]);
#  mean1 <- mean(group1[,(dataend-datastart)+1]);
#  mean2 <- mean(group2[,(dataend-datastart)+1]);
#  for (i in 1:extramodes) 
#  {
#  g1 <- rnorm(n1) * var1 + mean1;
#  g2 <- rnorm(n2) * var2 + mean2;
#
#   group1<-cbind(group1,g1);
#   group2<-cbind(group2,g2);
#  }
#}

# Find the means and covariances
xbar1<-apply(group1,2,mean);
xbar2<-apply(group2,2,mean);
xbarJoint<-apply(groupJoint,2,mean);
C1<-cov(group1);
C2<-cov(group2);
CJ<-cov(groupJoint);

I<-diag(1.0e-6,nrow=25);
C1inv<-solve(C1 + I);
C2inv<-solve(C2 + I);
CJinv<-solve(CJ + I);

# Eigen decompositions
W1 <- eigen(C1);
W2 <- eigen(C2);
WJ <- eigen(CJ);
v1 <- W1$values;
v2 <- W2$values;
vJ <- WJ$values;

# Since the cov matrix may not be full rank, remove tiny negatives
for (i in 1:numDims)
{
  if (v1[i] < 1.0e-6) v1[i] <- 0.0;
  if (v2[i] < 1.0e-6) v2[i] <- 0.0;  
  if (vJ[i] < 1.0e-6) vJ[i] <- 0.0;
}

# Normalize eigenvectors  -- already normalized by default
#  e <- as.vector(W1$vectors[,i]);
#  e <- e / sqrt(t(e)%*%e);

#noiserange <- (sqrt(v1[1]) + sqrt(v2[1])) / 2.0;

# Generate random data points in each distribution
for (i in 1:numA) {

  # multivariate sampling from normal dist
  b <- rnorm(numDims);
  c <- rnorm(numDims);
#  n <- W1$vectors%*%(rnorm(numDims)* noiserange * noiselevel);

  b <- W1$vectors%*%(b*sqrt(v1)) + xbar1;
  c <- WJ$vectors%*%(c*sqrt(vJ)) + xbarJoint;

  #Compute likelihood of point, mahalanobis
  md <-t(b-xbar1)%*%C1inv%*%(b-xbar1);
  q<-as.vector(groupJoint[i,]) - xbarJoint;
  mdr <-t(q)%*%CJinv%*%q;   

  if (i==1) 
  {
    Q<-rbind(t(b));
    R<-rbind(t(c));
    L<-rbind(sqrt(md));
    RL<-rbind(sqrt(mdr));
  }  
  else 
  {
    Q<-rbind(Q,t(b));
    R<-rbind(R,t(c));
    L<-c(L,sqrt(md));
    RL<-c(RL,sqrt(mdr));
  }
  if (i==1) groups<-1 else groups<-c(groups,1)
}

for (i in 1:numB) {
  # multivariate sampling from normal dist
  b <- rnorm(numDims);
  c <- rnorm(numDims);
#  n <- W2$vectors%*%(rnorm(numDims)* noiserange * noiselevel);

  b <- W2$vectors%*%(b*sqrt(v2)) + xbar2 ;
  c <- WJ$vectors%*%(c*sqrt(vJ)) + xbarJoint;

  # DEBUG: Compute likelihood of point, mahalanobis

   md <-t(b-xbar2)%*%C2inv%*%(b-xbar2);
   q<-as.vector(groupJoint[i+numA,]) - xbarJoint;
   mdr <-t(q)%*%CJinv%*%q;
  # END DEBUG

  Q<-rbind(Q,t(b));
  R<-rbind(R,t(c));
  L<-c(L,sqrt(md));
  RL<-c(RL,sqrt(mdr));

  groups<-c(groups,2);
}



# Store results in a data frame to return
#RES<-data.frame(Group=groups,Data=Q,Data.Joint=R,L=L,RL=RL)

RES<-data.frame(RL);
return(RES);

}


