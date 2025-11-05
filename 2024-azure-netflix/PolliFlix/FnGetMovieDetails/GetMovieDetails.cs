using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Cosmos;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.Functions.Worker.Http;
using Microsoft.Extensions.Logging;

namespace FnGetMovieDetails
{
    public class GetMovieDetails
    {
        private readonly ILogger<GetMovieDetails> _logger;
        private readonly CosmosClient _cosmosClient;

        public GetMovieDetails(ILogger<GetMovieDetails> logger, CosmosClient cosmosClient)
        {
            _logger = logger;
            _cosmosClient = cosmosClient;
        }

        [Function("detail")]
        public async Task<HttpResponseData> Run([HttpTrigger(AuthorizationLevel.Function, "get")] HttpRequestData req)
        {
            _logger.LogInformation("Get the movie detail from the Cosmos DB.");
            var container = _cosmosClient.GetContainer(Environment.GetEnvironmentVariable("COSMOS_DB_DATABASE_NAME"), Environment.GetEnvironmentVariable("COSMOS_DB_CONTAINER_NAME"));
            var id = req.Query["id"];
            var query = "SELECT * FROM c WHERE c.id = @id";
            var queryDefinition = new QueryDefinition(query).WithParameter("@id", id);
            var result = container.GetItemQueryIterator<MovieResult>(queryDefinition);
            var results = new List<MovieResult>();

            while (result.HasMoreResults)
            {
                var response = await result.ReadNextAsync();
                results.AddRange([.. response]);
            }

            var responseMessage = req.CreateResponse(System.Net.HttpStatusCode.OK);
            await responseMessage.WriteAsJsonAsync(results.FirstOrDefault());

            return responseMessage;
        }
    }
}
