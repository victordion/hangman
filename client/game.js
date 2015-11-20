function setCharAt(str,index,chr) {
  if(index > str.length-1) return str;
  return str.substr(0,index) + chr + str.substr(index+1);
}
/*
 * Check the guess against true_sentence
 */ 
function checkGuess(true_sentence, masked_sentence, guessed_content, wrong_guesses) {
  var score = 0;
  var num_masked = 0;
  var complete = false;

  for(var i = 0; i < guessed_content.length; ++i) {
    num_masked += masked_sentence[i] == '?';
  }

  /*
   *  If the user if guessing a whole sentence.
   */
  if(guessed_content.length > 1) {
    if(guessed_content == true_sentence) {
      feedback = 'Congratulations! Sentence guess completed!';
      masked_sentence = true_sentence;
      score += num_masked;
      /* Actually it is a correct guess, but last guess, so add it anyway */
      wrong_guesses.push(guessed_content);
    }
    else {
      feedback = "Incorrect sentence guess: " + guessed_content;
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
       score += 1;
     }
     else {
       feedback = 'You guessed '.concat(guessed_content, ', which is wrong.');
       score -= 1;
     }
  }
  if(masked_sentence ==  true_sentence) {
    complete = true;
  }
  
  return {
    'score' : score,
    'complete' : complete,
    'feedback': feedback,
    'masked_sentence': masked_sentence,
    'wrong_guesses': wrong_guesses
  };
}

var masked_sentence = '';
var true_sentence = '';
var wrong_guesses = [];
var hostAddress = 'http://128.230.213.57:11000';
var app = angular.module('hangmanApp', []);
var State = {IDLE : "IDLE", IN_PROCESS: "IN_PROCESS"};
var curr_state;

app.controller('hangmanCtrl', ['$scope', '$http', function($scope, $http) {
  
  /*
   * Set button to be disabled/not-disabled
   */
  var update_button_disable_features = function() {
    $scope.request_guess_button_disabled = (curr_state == State.IDLE); 
    $scope.check_guess_button_disabled = (curr_state == State.IDLE);
    console.log("request_guess_button_disabled: " + $scope.request_guess_button_disabled);
    console.log("check_guess_button_disabled: " + $scope.check_guess_button_disabled);   
  }

  /*
   * Set game state variables to their initial values
   */
  var init = function() {
    console.log("init() called");
    curr_state = State.IDLE;
    update_button_disable_features(); 
  
    $scope.masked_sentence = '';      
    $scope.true_sentence = '';      
	  $scope.wrong_guesses = [];
    $scope.human_score = 0;
    $scope.computer_score = 0;
    $scope.human_feedback = '';
    $scope.computer_feedback = '';
  }
 
  init();
 

  /*
   * Send a HTTP request ask for new sentence (start a new game)
   */
  $scope.requestSentence = function() {
    init(); 
    var req = {
      method: 'POST',
      url: hostAddress,
      headers: {'Content-Type': undefined },
      data:{'request_type':'request_sentence',
            'sentence_id': $scope.sentence_id }
     
    }
    
    console.log('Request sent: ' + JSON.stringify(req));
    
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
      curr_state = State.IN_PROCESS;
      update_button_disable_features();
    });
  };
  
  /*
   * Send an HTTP request asking for a server side guess
   */
  $scope.requestAIGuess = function() {
    var req = {
      method: 'POST',
      url: hostAddress,
      headers: {
        'Content-Type': undefined
      },
      data: {
        'request_type' : 'request_guess',
        'masked_sentence' : $scope.masked_sentence,
        'wrong_guesses' : $scope.wrong_guesses,
      }
    }
    console.log('Request sent: ' + JSON.stringify(req));
    
    $http(req).then(function(response){
      console.log('Server reply result: ' + response);
      var json = response.data;
      var guessed = json['guess_result'];
      console.log('Server guess result: ' + guessed);

      var ret = checkGuess($scope.true_sentence, $scope.masked_sentence, guessed, $scope.wrong_guesses);    
      $scope.computer_feedback = ret.feedback;
      $scope.masked_sentence = ret.masked_sentence;
      $scope.wrong_guesses = ret.wrong_guesses;
      $scope.computer_score += ret.score;
      if(ret.complete) {
          curr_state = State.IDLE;
          update_button_disable_features();    
      }
    });
  };

  /*
   * Check the answer after the human user makes a guess
   */
  $scope.checkGuess = function() {
    if($scope.guessed_content) { 
      var ret = checkGuess($scope.true_sentence, $scope.masked_sentence, $scope.guessed_content, $scope.wrong_guesses);
      $scope.human_feedback = ret.feedback;
      $scope.masked_sentence = ret.masked_sentence;
      $scope.wrong_guesses = ret.wrong_guesses;
      $scope.human_score += ret.score;
      $scope.guessed_content = '';
      if(ret.complete) {
          curr_state = State.IDLE;    
          update_button_disable_features();
      }
    }
  };
  
}]);
 
