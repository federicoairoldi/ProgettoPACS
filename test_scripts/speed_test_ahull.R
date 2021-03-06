# Federico Airoldi    matricola: 892377   codice persona: 10484065
#
# this script tests the performances of the package alphahull with respect to the
# performances of the R/C++ hybrid package RcppAlphahull for the function ashape
#
# by setting a different number of sites the script computes the Voronoi diagram and Delanuay tesselation
# of n randomized point of R2 in [0;1]x[0;1] (if one likes, he can change the seed) and next computes the
# alpha hull complement for a random value of alpha.
#
# NB: this script compares speed of constructions of the alpha hull only, it doesn't consider the time 
# spent to compute the Voronoi tesselation/Delanuay triangulation.

require(rbenchmark)
require(alphahull)
require(RcppAlphahull)
if( getwd()!=dirname(rstudioapi::getActiveDocumentContext()$path) )
  setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

print("TESTING SPEED OF AHULL")

set.seed(3)
n.nodes = seq(100, 1000, by = 100)
RTime = c()
CppTime = c()

for(n in n.nodes){
  print(paste("Simulation:",n))
  x = runif(n)
  y = runif(n)
  alpha = runif(1)
  vorcpp = RcppAlphahull::delvor(x, y)
  vorR = alphahull::delvor(x, y)
  res = benchmark("Cpp" = RcppAlphahull::ahull(vorcpp, alpha = alpha),
                  "R" = alphahull::ahull(vorR, alpha = alpha),
                  replications = 1)
  CppTime = rbind(CppTime, res[which(res[,"test"] == "Cpp"), c("elapsed", "user.self", "sys.self")])
  RTime = rbind(RTime, res[which(res[,"test"] == "R"), c("elapsed", "user.self", "sys.self")])
}

Cpp.User = CppTime[,2]
R.User = RTime[,2]
transf = n.nodes*log(n.nodes, 2)
fit.cpp = lm(Cpp.User ~ transf)
summary(fit.cpp)
fit.R = lm(R.User ~ transf)
summary(fit.R)

x11()
png("/home/federico/Dropbox/Alpha-hulls/Documenti/img/speed_ahull.png")
par(mar = c(5,6,1,1))
plot(c(n.nodes, n.nodes), c(Cpp.User, R.User), xlab = "number of sites", ylab = "CPU time", cex.lab = 1.5)
points(n.nodes, Cpp.User, col = "blue", pch = 19)
points(n.nodes, R.User, col = "red", pch = 19)
lines(n.nodes, fit.cpp$fitted.values, col = "green", lty = 2)
lines(n.nodes, fit.R$fitted.values, col = "orange", lty = 2)
legend("topleft", legend = c("C++", "R", "lm fit C++", "lm fit R"), fill = c("blue", "red", "green", "orange"), cex = 1.5)
graphics.off()

R.User/Cpp.User
