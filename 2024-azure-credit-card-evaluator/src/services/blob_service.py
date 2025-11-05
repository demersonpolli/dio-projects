import os
import streamlit as st
from azure.storage.blob import BlobServiceClient
from utils.Config import Config

def upload_blob(file, file_name):
    try:
        st.write('zero')
        blob_service_client= BlobServiceClient.from_connection_string(Config.AZURE_STORAGE_CONNECTION_STRING)
        st.write('one')
        blob_client= blob_service_client.get_blob_client(container= Config.CONTAINER_NAME, blob= file_name)
        st.write('two')
        blob_client.upload_blob(file, overwrite= True)
        st.write('three')
        return blob_client.url
    except Exception as ex:
        st.error(f"Erro ao enviar o arquivo para o Azure Blob Storage: {ex}")
        return None
    