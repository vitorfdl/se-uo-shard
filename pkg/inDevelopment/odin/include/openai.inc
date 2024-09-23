
use uo;
use os;
use basic;


enum OPENAI_MODELS
   heavy_model := "gpt-4o",
   faster_model := "gpt-4o-mini",
endenum

/*
 * Function: OpenAICompletion
 * Description: This function takes a system message and an array of questions as input. It formats the questions and sends a POST request to the OpenAI API. 
 * The API response is returned as a result. If the input types are not as expected, an error message is returned.
 * 
 * Parameters:
 *    systemMessage: A string representing the system message.
 *    questions: An array of questions where each question is a struct containing the keys 'assistant' and 'user'.
 *    options: A struct containing the options for the API request. Refer to https://platform.openai.com/docs/api-reference/chat/create for more information.
 *        options["model"]: The model to use for the API request. Defaults to GPT_4o_mini.
 *        options["max_tokens"]: The maximum number of tokens to generate. Defaults to null.
 *        options["temperature"]: What sampling temperature to use. Higher values means the model will take more risks. Try 0.9 for more creative applications, and 0 (argmax sampling) for ones with a well-defined answer. Defaults to null.
 *        options["top_p"]: An alternative to sampling with temperature, called nucleus sampling, where the model considers the results of the tokens with top_p probability mass. So 0.1 means only the tokens comprising the top 10% probability mass are considered. Defaults to null.
 *        options["presence_penalty"]: What penalty to apply if a token is already present at all. Bigger values mean the model is less likely to repeat itself. Defaults to null.
 *        options["frequency_penalty"]: What penalty to apply if a token has already been generated in the past. Bigger values mean the model is less likely to repeat itself. Defaults to null.
 *        options["json"]: Whether to return the API response as a struct or a string. Defaults to false.
 *
 * Example code:
 *  ```
 *  var exampleSystemMessage := "This is a system message example";
 *  var exampleQuestions := array{ struct{ "assistant" := "This is an assistant question example" }, struct{ "user" := "This is a user question example" } };
 *  var exampleResult := OpenAICompletion(exampleSystemMessage, exampleQuestions);
 *  if (exampleResult["error"])
 *     return struct{ "error" := "Example error message" };
 *  endif
 *  Print(exampleResult);
 *  ```
 *
 * Returns: 
 *    A struct containing the API response or an error message.
 */

function openAICompletion(systemMessage, questions, options := struct{})
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
      "model" := options["model"] ? options["model"] : faster_model,
      "messages" := formatedQuestions,
      "max_tokens" := options["max_tokens"] ? options["max_tokens"] : "",
      "temperature" := options["temperature"] ? options["temperature"] : "",
      "top_p" := options["top_p"] ? options["top_p"] : "",
      "presence_penalty" := options["presence_penalty"] ? options["presence_penalty"] : "",
      "frequency_penalty" := options["frequency_penalty"] ? options["frequency_penalty"] : "",
      "response_format" := options["json"] ? struct{ "type" := "json_object"} : ""
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

   if (unpackedJSON["error"])
      return struct{ "error" := unpackedJSON["error"]["message"] };
   endif

   return unpackedJSON["choices"][1]["message"]["content"];
endfunction

/*
 * Function: isPromptSafe
 * Description: This function takes a prompt as input and sends a POST request to the OpenAI API. 
 * The API response is returned as a result. If the input types are not as expected, an error message is returned.
 * Parameters:
 *    prompt: A string representing the prompt to be checked for moderation.
 * Returns: 
 *    A boolean value indicating whether the prompt is flagged as containing harmful content.
 */
function isPromptSafe(prompt)
   var apiURL := "https://api.openai.com/v1/moderations";
   var method := "POST";
   var jsonData := PackJSON(struct{
      "input" := prompt
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
   
   if (unpackedJSON["error"])
      return struct{ "error" := unpackedJSON["error"]["message"] };
   endif

   return unpackedJSON["results"][0]["flagged"];
endfunction