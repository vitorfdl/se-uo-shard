
use uo;
use os;
use basic;

/*
 * Function: OpenAICompletion
 * Description: This function takes a system message and an array of questions as input. It formats the questions and sends a POST request to the OpenAI API. 
 * The API response is returned as a result. If the input types are not as expected, an error message is returned.
 * Parameters:
 *    systemMessage: A string representing the system message.
 *    questions: An array of questions where each question is a struct containing the keys 'assistant' and 'user'.
 *
 * Example code:
 *  var exampleSystemMessage := "This is a system message example";
 *  var exampleQuestions := array{ struct{ "assistant" := "This is an assistant question example" }, struct{ "user" := "This is a user question example" } };
 *  var exampleResult := OpenAICompletion(exampleSystemMessage, exampleQuestions);
 *  if (exampleResult["error"])
 *     return struct{ "error" := "Example error message" };
 *  endif
 *
 * Returns: 
 *    A struct containing the API response or an error message.
 */

function OpenAICompletion(systemMessage, questions)
   if (typeof(systemMessage) != "String")
      return struct{ "error" := "systemMessage must be a string" };
   endif

   if (typeof(questions) != "Array")
      return struct{ "error" := "questions must be an array" };
   endif

   var formatedQuestions := array{ struct{
      "role" := "system",
      "content" := systemMessage
   } };

   foreach question in questions
      if (typeof(question) != "Struct" || (!question.exists("assistant") && !question.exists("user")))
         return struct{ "error" := "Each question must be a struct containing the keys 'assistant' and 'user'" };
      endif

      if (question.exists("assistant"))
         formatedQuestions.append(struct{
            "role" := "assistant",
            "content" := question["assistant"]
         });
      endif

      if (question.exists("user"))
         formatedQuestions.append(struct{
            "role" := "user",
            "content" := question["user"]
         });
      endif
   endforeach

   var apiURL := "https://api.openai.com/v1/chat/completions";
   var method := "POST";
   var jsonData := PackJSON(struct{
      "model" := "gpt-3.5-turbo-1106",
      "messages" := formatedQuestions
   });

   var apiResponse := HTTPRequest(
         apiURL,
         method,
         struct{
            data := jsonData,
            headers := struct{
               "Content-Type" := "application/json",
               "Authorization" := "Bearer {1}".format(GetEnvironmentVariable("OPENAI_KEY"))
            }
         }
      );
   var unpackedJSON := UnpackJSON(apiResponse);

   return unpackedJSON["choices"][1]["message"]["content"];
endfunction
