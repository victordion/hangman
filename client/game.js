function setCharAt(str,index,chr) {
  if(index > str.length-1) return str;
  return str.substr(0,index) + chr + str.substr(index+1);
}
  
function checkGuess(true_sentence, masked_sentence, guessed_content, wrong_guesses) {
  /*
   *  If the user if guessing a whole sentence.
   */
  if(guessed_content.length > 1) {
    if(guessed_content == true_sentence) {
      feedback = 'Congratulations! You win!';
    }
    else {
      feedback = "Incorrect sentence guess";
      if(wrong_guesses.indexOf(guessed_content) == -1) {
        wrong_guesses.push(guessed_content);
      }
    }
  }
  /*
   *  A repetitive character guess
   */
  else if(wrong_guesses.indexOf(guessed_content) > - 1) {
    feedback = 'You guessed '.concat(guessed_content, ', which is already guessed');
  }
  /*
   *  A new character guess
   */
  else {
     wrong_guesses.push(guessed_content);
     var hit = false;
     for(var i = 0; i < true_sentence.length; i++) {
       if(true_sentence[i] == guessed_content) {
         masked_sentence = setCharAt(masked_sentence, i, guessed_content);
         hit = true;
       }
     }
     if(hit) {
       feedback = 'You guessed '.concat(guessed_content, ', which is a hit. Congratulations!');
     }
     else {
       feedback = 'You guessed '.concat(guessed_content, ', which is wrong.');
     }
  }
  if(masked_sentence ==  true_sentence) {
    feedback = 'Congratulations! You win!';
  }
  
  return {
    'feedback': feedback,
    'masked_sentence': masked_sentence,
    'wrong_guesses': wrong_guesses
  };
}

var masked_sentence = '';
var true_sentence = '';
var wrong_guesses = [];
var hostAddress = 'http://127.0.0.1:11000';
var app = angular.module('hangmanApp', []);

app.controller('hangmanCtrl', ['$scope', '$http', function($scope, $http) {
  
  $scope.masked_sentence = '';      
  $scope.true_sentence = '';      
	$scope.wrong_guesses = [];

  /*
   * Send a HTTP request ask for new sentence
   */
  $scope.requestSentence = function() {
    var req = {
      method: 'POST',
      url: hostAddress,
      headers: {'Content-Type': undefined },
      data:{'request_type':'request_sentence'}
    }
    
    console.log('Request sent: ' + JSON.stringify(req));
    
    $scope.masked_sentence = '';

    $http(req).then(function(response){
      console.log('Response Received: ' + JSON.stringify(response));
      var json = response.data;
      $scope.true_sentence = json['new_sentence'];
      console.log('Current true_sentence: ' + $scope.true_sentence);
      for(var i = 0; i < $scope.true_sentence.length; ++i) {
        if($scope.true_sentence[i] != ' ') { 
          $scope.masked_sentence += '?';
        }
        else {
          $scope.masked_sentence += ' ';
        }
      }
    });
     
   /* 
    true_sentence =  'i am a good man';
    masked_sentence ='? ?? ? ???? ???'; 
    $scope.masked_sentence = masked_sentence;
    $scope.true_sentence = true_sentence;
   */
  };
  
  /*
   * Send an HTTP request asking for a server side guess
   */
  $scope.requestAIGuess = function() {
    var req = {
      method: 'POST',
      url: 'http://127.0.0.1:11000',
      headers: {
        'Content-Type': undefined
      },
      data: {
        'request_type' : 'request_guess',
        'masked_sentence' : masked_sentence,
        'wrong_guesses' : wrong_guesses,
      }
    }
    $http(req).then(function(response){
      var json = response.data;
      var obj = JSON.parse(json);
      var guessed = obj['guessed_result'];
      

    });
  };

  /*
   * Check the answer after the human user makes a guess
   */
  $scope.checkGuessedContent = function() {
    
    var ret = checkGuess($scope.true_sentence, $scope.masked_sentence, $scope.guessed_content, $scope.wrong_guesses);
    $scope.human_feedback = ret.feedback;
    $scope.masked_sentence = ret.masked_sentence;
    $scope.wrong_guesses = ret.wrong_guesses;

  };
  
}]);
 
