let b = {h:1,i:2};      
let c = {h:1,i:2};   
let yes = 1;
for( const v of Object.entries(b)){
  if(c[v[0]]==v[1]) continue;
  else{
    yes = 0;
    break;
  }
}
console.log(yes);

