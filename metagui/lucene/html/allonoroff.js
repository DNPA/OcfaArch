
function allon(inForm, inSize){

  allonoroff(inForm, inSize, true);
}

function alloff(inForm, inSize){

  allonoroff(inForm, inSize, false);
}

function allonoroff(inForm, inSize, shouldBeOn){
  
  var x; 
  //  alert('inForm is ' + inForm);
  //alert("inForm['indexdir'] is " + inForm['indexdir']);
  inForm['indexdir'].checked = shouldBeOn;
  for (x = 0; x < inSize -1; x++){

    inForm['indexdir' + x].checked = shouldBeOn;
  }

}
