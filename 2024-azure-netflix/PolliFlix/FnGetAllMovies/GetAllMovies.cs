using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Cosmos;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Azure.Functions.Worker.Http;
using Microsoft.Extensions.Logging;

namespace FnGetAllMovies
{
    public class GetAllMovies
    {
        private readonly ILogger<GetAllMovies> _logger;
        private readonly CosmosClient _cosmosClient;

        public GetAllMovies(ILogger<GetAllMovies> logger, CosmosClient cosmosClient)
        {
            _logger = logger;
            _cosmosClient = cosmosClient;
        }

        [Function("all")]
        public async Task<HttpResponseData> Run([HttpTrigger(AuthorizationLevel.Function, "get")] HttpRequestData req)
        {
            _logger.LogInformation("Get details of all movies from Cosmos DB.");
            var container = _cosmosClient.GetContainer(Environment.GetEnvironmentVariable("COSMOS_DB_DATABASE_NAME"), Environment.GetEnvironmentVariable("COSMOS_DB_CONTAINER_NAME"));
            var id = req.Query["id"];
            var query = "SELECT * FROM c";
            var queryDefinition = new QueryDefinition(query);
            var result = container.GetItemQueryIterator<MovieResult>(queryDefinition);
            var results = new List<MovieResult>();

            while (result.HasMoreResults)
            {
                var response = await result.ReadNextAsync();
                results.AddRange([.. response]);
            }

            var responseMessage = req.CreateResponse(System.Net.HttpStatusCode.OK);
            await responseMessage.WriteAsJsonAsync(results);

            return responseMessage;
        }
    }
}
