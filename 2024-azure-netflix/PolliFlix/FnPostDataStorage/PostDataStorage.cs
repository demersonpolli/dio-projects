using Azure.Storage.Blobs;
using Azure.Storage.Blobs.Models;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.Functions.Worker;
using Microsoft.Extensions.Logging;

namespace FnPostDataStorage
{
    public class PostDataStorage
    {
        private readonly ILogger<PostDataStorage> _logger;

        public PostDataStorage(ILogger<PostDataStorage> logger)
        {
            _logger = logger;
        }

        [Function("dataStorage")]
        public async Task<IActionResult> Run([HttpTrigger(AuthorizationLevel.Function, "post")] HttpRequest req)
        {
            _logger.LogInformation("Uploading the image on the Storage Account.");
            
            if (!req.Headers.TryGetValue("file-type", out var fileTypeHeader))
            {
                return new BadRequestObjectResult("Please provide the file type in the header.");
            }

            var fileType = fileTypeHeader.ToString();
            var form = await req.ReadFormAsync();
            var file = form.Files["file"];

            if (file == null || file.Length == 0)
            {
                return new BadRequestObjectResult("Please provide the file in the form data.");
            }

            string connectionString = Environment.GetEnvironmentVariable("AzureWebJobsStorage");
            string containerName = $"{fileType}s";
            string blobName = $"{Guid.NewGuid()}{Path.GetExtension(file.FileName)}";
            BlobClient blobClient = new(connectionString, containerName, file.FileName);
            BlobContainerClient containerClient = new(connectionString, containerName);

            await containerClient.CreateIfNotExistsAsync();
            await containerClient.SetAccessPolicyAsync(PublicAccessType.BlobContainer);

            var blob = containerClient.GetBlobClient(blobName);
            using (var stream = file.OpenReadStream())
            {
                await blob.UploadAsync(stream, true);
            }

            _logger.LogInformation($"Image '{file.FileName}' uploaded successfully on the Storage Account.");

            return new OkObjectResult(new
            {
                Message = "Image uploaded successfully.",
                BlobUri = blob.Uri
            });
        }
    }
}
