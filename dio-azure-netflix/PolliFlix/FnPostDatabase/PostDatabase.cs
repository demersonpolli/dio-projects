using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace FnPostDatabase
{
    public class PostDatabase
    {
        private readonly ILogger<PostDatabase> _logger;

        public PostDatabase(ILogger<PostDatabase> logger)
        {
            _logger = logger;
        }

        [Function("movie")]
        [CosmosDBOutput("%COSMOS_DB_DATABASE_NAME%", "%COSMOS_DB_CONTAINER_NAME%", PartitionKey = "%COSMOS_DB_PARTITION_KEY%", Connection = "COSMOS_DB_CONNECTION", CreateIfNotExists = true)]
        public async Task<object?> Run([HttpTrigger(AuthorizationLevel.Function, "post")] HttpRequest req)
        {
            _logger.LogInformation("Storing movie data into Cosmos DB.");

            MovieRequest? movie = null;
            var content = await new StreamReader(req.Body).ReadToEndAsync();

            try
            {
                movie = JsonConvert.DeserializeObject<MovieRequest>(content);

                if (movie == null)
                {
                    return new BadRequestObjectResult("Please provide the movie details in the request body.");
                }
            }
            catch (Exception ex)
            {
                return new BadRequestObjectResult($"Invalid request: {ex.Message}");
            }

            return JsonConvert.SerializeObject(movie);
        }
    }
}
