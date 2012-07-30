#include <stdio.h>
#include <stdlib.h>
#include "xmlhandler.h"
#include "JSR309Manager.h"
#include "MediaSession.h"
#include "codecs.h"
#include "amf.h"

xmlrpc_value* EventQueueCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser nameParser;
        JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	//Create the event queue
	int queueId = jsr->CreateEventQueue();

	//Si error
	if (!queueId>0)
		return xmlerror(env,"Could not create event queue");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",queueId));
}

xmlrpc_value* EventQueueDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	 //Parseamos
	int queueId;
	xmlrpc_parse_value(env, param_array, "(i)", &queueId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Delete event queue
	if (!jsr->DeleteEventQueue(queueId))
		return xmlerror(env,"Event queue does not exist\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* MediaSessionCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser nameParser;
        JSR309Manager *jsr = (JSR309Manager*)user_data;

	 //Parseamos
	char *str;
	int queueId;
        xmlrpc_parse_value(env, param_array, "(si)", &str,&queueId);

	//Parse string
	nameParser.Parse((BYTE*)str,strlen(str));

	//Creamos la sessionerencia
	int sessionId = jsr->CreateMediaSession(nameParser.GetWString(),queueId);

	//Si error
	if (!sessionId>0)
		return xmlerror(env,"Could not create session");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",sessionId));
}

xmlrpc_value* MediaSessionDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	 //Parseamos
	int sessionId;
	xmlrpc_parse_value(env, param_array, "(i)", &sessionId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Delete sessionerence
	if (!jsr->DeleteMediaSession(sessionId))
		return xmlerror(env,"Session does not exist");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(is)", &sessionId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int playerId = session->PlayerCreate(parser.GetWString());

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(playerId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",playerId));
}

xmlrpc_value* PlayerDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerDelete(playerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerOpen(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	char *filename;
	xmlrpc_parse_value(env, param_array, "(iis)", &sessionId, &playerId,&filename);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerOpen(playerId,filename);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not open player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerPlay(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerPlay(playerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not play player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerSeek(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	int time;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId, &playerId,&time);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerSeek(playerId,time);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not seek player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerStop(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerStop(playerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not stop player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* PlayerClose(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int playerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->PlayerClose(playerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not stop player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(is)", &sessionId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create recorder
	int recorderId = session->RecorderCreate(parser.GetWString());

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(recorderId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",recorderId));
}

xmlrpc_value* RecorderDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int recorderId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &recorderId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->RecorderDelete(recorderId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete recorder\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderRecord(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int recorderId;
	char *filename;
	xmlrpc_parse_value(env, param_array, "(iis)", &sessionId, &recorderId, &filename);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->RecorderRecord(recorderId,filename);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not play recorder\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderStop(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int recorderId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId,&recorderId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->RecorderStop(recorderId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not stop recorder\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderAttachToEndpoint(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int recorderId;
	int endpointId;
	int media;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&recorderId,&endpointId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->RecorderAttachToEndpoint(recorderId,endpointId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderAttachToAudioMixerPort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int recorderId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&recorderId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->RecorderAttachToAudioMixerPort(recorderId,mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderAttachToVideoMixerPort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int recorderId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&recorderId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->RecorderAttachToVideoMixerPort(recorderId,mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* RecorderDettach(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int recorderId;
	int media;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&recorderId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->RecorderDettach(recorderId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	char *str;
	int audioSupported;
	int videoSupported;
	int textSupported;
	xmlrpc_parse_value(env, param_array, "(isbbb)", &sessionId,&str,&audioSupported, &videoSupported, &textSupported);


	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int endpointId = session->EndpointCreate(parser.GetWString(),audioSupported,videoSupported,textSupported);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(endpointId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",endpointId));
}

xmlrpc_value* EndpointDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int endpointId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &endpointId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->EndpointDelete(endpointId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete endpoint\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointStartSending(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	int res = 0;
	char *sendIp;
	int sendPort;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iiisiS)", &sessionId,&endpointId,&media,&sendIp,&sendPort,&rtpMap);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Depending on the media
	switch((MediaFrame::Type)media)
	{
		case MediaFrame::Video:
		{
			//Get the rtp map
			VideoCodec::RTPMap map;

			//Get map size
			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (VideoCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}

			//Start sending video
			res = session->EndpointStartSendingVideo(endpointId,sendIp,sendPort,map);

			//Continue
			break;
		}
		case MediaFrame::Audio:
		{
			//Get the rtp map
			AudioCodec::RTPMap map;

			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (AudioCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}
			//Start sending audio
			res = session->EndpointStartSendingAudio(endpointId,sendIp,sendPort,map);
			//Continue
			break;
		}
		case MediaFrame::Text:
		{
			//Get the rtp map
			TextCodec::RTPMap map;

			//Get map size
			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (TextCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}
			//Start sending text
			res = session->EndpointStartSendingText(endpointId,sendIp,sendPort,map);
			//Continue
			break;
		}
	}

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointStopSending(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	int res = 0;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&endpointId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Depending on the media
	switch((MediaFrame::Type)media)
	{
		case MediaFrame::Video:
			//Stop sending video
			res = session->EndpointStopSendingVideo(endpointId);
			//Continue
			break;
		case MediaFrame::Audio:
			//Stop sending video
			res = session->EndpointStopSendingAudio(endpointId);
			//Continue
			break;
		case MediaFrame::Text:
			//Stop sending text
			res = session->EndpointStopSendingText(endpointId);
			//Continue
			break;
	}

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"No se ha podido terminar la sessionerencia\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointStartReceiving(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	int recPort = 0;
	xmlrpc_value *rtpMap;
	xmlrpc_parse_value(env, param_array, "(iiiS)", &sessionId,&endpointId,&media,&rtpMap);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Depending on the media
	switch((MediaFrame::Type)media)
	{
		case MediaFrame::Video:
		{
			//Get the rtp map
			VideoCodec::RTPMap map;

			//Get map size
			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (VideoCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}

			//Start receiving video and get listening port
			recPort = session->EndpointStartReceivingVideo(endpointId,map);

			//Continue
			break;
		}
		case MediaFrame::Audio:
		{
			//Get the rtp map
			AudioCodec::RTPMap map;

			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (AudioCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}
			//Start receiving audio and get listening port
			recPort = session->EndpointStartReceivingAudio(endpointId,map);
			//Continue
			break;
		}
		case MediaFrame::Text:
		{
			//Get the rtp map
			TextCodec::RTPMap map;

			//Get map size
			int j = xmlrpc_struct_size(env,rtpMap);

			//Parse rtp map
			for (int i=0;i<j;i++)
			{
				xmlrpc_value *key, *val;
				const char *type;
				int codec;
				//Read member
				xmlrpc_struct_read_member(env,rtpMap,i,&key,&val);
				//Read name
				xmlrpc_parse_value(env,key,"s",&type);
				//Read value
				xmlrpc_parse_value(env,val,"i",&codec);
				//Add to map
				map[atoi(type)] = (TextCodec::Type) codec;
				//Decrement ref counter
				xmlrpc_DECREF(key);
				xmlrpc_DECREF(val);
			}
			//Start receiving text and get listening port
			recPort = session->EndpointStartReceivingText(endpointId,map);
			//Continue
			break;
		}
	}

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!recPort)
		return xmlerror(env,"No se ha podido terminar la sessionerencia\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",recPort));
}

xmlrpc_value* EndpointStopReceiving(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	int res = 0;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&endpointId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Depending on the media
	switch((MediaFrame::Type)media)
	{
		case MediaFrame::Video:
			//Stop sending video
			res = session->EndpointStopReceivingVideo(endpointId);
			//Continue
			break;
		case MediaFrame::Audio:
			//Stop sending video
			res = session->EndpointStopReceivingAudio(endpointId);
			//Continue
			break;
		case MediaFrame::Text:
			//Stop sending text
			res = session->EndpointStopReceivingText(endpointId);
			//Continue
			break;
	}
	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointRequestUpdate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	int res = 0;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&endpointId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Stop sending text
	res = session->EndpointRequestUpdate(endpointId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointAttachToPlayer(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int playerId;
	int media;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&endpointId,&playerId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->EndpointAttachToPlayer(endpointId,playerId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointAttachToEndpoint(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int sourceId;
	int media;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&endpointId,&sourceId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->EndpointAttachToEndpoint(endpointId,sourceId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointAttachToAudioMixerPort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&endpointId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->EndpointAttachToAudioMixerPort(endpointId,mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointAttachToVideoMixerPort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&endpointId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->EndpointAttachToVideoMixerPort(endpointId,mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* EndpointDettach(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int endpointId;
	int media;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&endpointId,&media);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->EndpointDettach(endpointId,(MediaFrame::Type)media);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AudioMixerCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(is)", &sessionId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int mixerId = session->AudioMixerCreate(parser.GetWString());

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(mixerId<0)
		return xmlerror(env,"Could not create media mixer\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mixerId));
}

xmlrpc_value* AudioMixerDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &mixerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->AudioMixerDelete(mixerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AudioMixerPortCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(iis)", &sessionId, &mixerId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int portId = session->AudioMixerPortCreate(mixerId,parser.GetWString());

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(portId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",portId));
}


xmlrpc_value* AudioMixerPortSetCodec(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int codec;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId, &mixerId, &portId, &codec);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->AudioMixerPortSetCodec(mixerId,portId,(AudioCodec::Type)codec);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete player\n");

	//Devolvemos el resultado
	return xmlok(env);
}


xmlrpc_value* AudioMixerPortDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId, &mixerId, &portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->AudioMixerPortDelete(mixerId,portId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete mixer port\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AudioMixerPortAttachToPlayer(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int playerId;

	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&portId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->AudioMixerPortAttachToPlayer(mixerId,portId,playerId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AudioMixerPortAttachToEndpoint(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int endpointId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&portId,&endpointId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->AudioMixerPortAttachToEndpoint(mixerId,portId,endpointId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* AudioMixerPortDettach(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->AudioMixerPortDettach(mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	char *str;
	xmlrpc_parse_value(env, param_array, "(is)", &sessionId, &str);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int mixerId = session->VideoMixerCreate(parser.GetWString());

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(mixerId<0)
		return xmlerror(env,"Could not create media mixer\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mixerId));
}

xmlrpc_value* VideoMixerDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	xmlrpc_parse_value(env, param_array, "(ii)", &sessionId, &mixerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->VideoMixerDelete(mixerId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete player\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerPortCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	char *str;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(iisi)", &sessionId, &mixerId, &str, &mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Parse string
	parser.Parse((BYTE*)str,strlen(str));

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int portId = session->VideoMixerPortCreate(mixerId,parser.GetWString(),mosaicId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(portId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",portId));
}

xmlrpc_value* VideoMixerPortDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId, &mixerId, &portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->VideoMixerPortDelete(mixerId,portId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete mixer port\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerPortSetCodec(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	UTF8Parser parser;
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int codec;
	int size;
	int fps;
	int bitrate;
	int intraPeriod;
	xmlrpc_parse_value(env, param_array, "(iiiiiiii)", &sessionId, &mixerId, &portId, &codec, &size, &fps, &bitrate, &intraPeriod);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int res = session->VideoMixerPortSetCodec(mixerId,portId,(VideoCodec::Type)codec,size,fps,bitrate,intraPeriod);
	
	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerPortAttachToPlayer(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int playerId;

	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&portId,&playerId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerPortAttachToPlayer(mixerId,portId,playerId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerPortAttachToEndpoint(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int endpointId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&portId,&endpointId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerPortAttachToEndpoint(mixerId,portId,endpointId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerPortDettach(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&mixerId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerPortDettach(mixerId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicCreate(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int comp;
	int size;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId, &mixerId, &comp, &size);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//Create player
	int mosaicId = session->VideoMixerMosaicCreate(mixerId,(Mosaic::Type)comp,size);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(mosaicId<0)
		return xmlerror(env,"Could not set input token for media bridge\n");

	//Devolvemos el resultado
	return xmlok(env,xmlrpc_build_value(env,"(i)",mosaicId));
}

xmlrpc_value* VideoMixerMosaicDelete(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	MediaSession *session;
	JSR309Manager *jsr = (JSR309Manager*)user_data;

	//Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId, &mixerId, &mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"Media session not found\n");

	//La borramos
	bool res = session->VideoMixerMosaicDelete(mixerId,mosaicId);

	//Release ref
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Could not delete mixer port\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicSetSlot(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int portId;
	int mosaicId;
	int num;

	xmlrpc_parse_value(env, param_array, "(iiiii)", &sessionId,&mixerId,&mosaicId,&num,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicSetSlot(mixerId,mosaicId,num,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicSetCompositionType(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	int comp;
	int size;
	xmlrpc_parse_value(env, param_array, "(iiiii)", &sessionId,&mixerId,&mosaicId,&comp,&size);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicSetCompositionType(mixerId,mosaicId,(Mosaic::Type)comp,size);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicSetOverlayPNG(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	char* overlay;
	xmlrpc_parse_value(env, param_array, "(iiis)", &sessionId,&mixerId,&mosaicId,&overlay);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicSetOverlayPNG(mixerId,mosaicId,overlay);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicResetOverlay(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	xmlrpc_parse_value(env, param_array, "(iii)", &sessionId,&mixerId,&mosaicId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicResetSetOverlay(mixerId,mosaicId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicAddPort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&mosaicId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicAddPort(mixerId,mosaicId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

xmlrpc_value* VideoMixerMosaicRemovePort(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
	JSR309Manager *jsr = (JSR309Manager*)user_data;
	MediaSession *session = NULL;

	 //Parseamos
	int sessionId;
	int mixerId;
	int mosaicId;
	int portId;
	xmlrpc_parse_value(env, param_array, "(iiii)", &sessionId,&mixerId,&mosaicId,&portId);

	//Comprobamos si ha habido error
	if(env->fault_occurred)
		return 0;

	//Obtenemos la referencia
	if(!jsr->GetMediaSessionRef(sessionId,&session))
		return xmlerror(env,"The media Session does not exist");

	//La borramos
	int res = session->VideoMixerMosaicRemovePort(mixerId,mosaicId,portId);

	//Liberamos la referencia
	jsr->ReleaseMediaSessionRef(sessionId);

	//Salimos
	if(!res)
		return xmlerror(env,"Error\n");

	//Devolvemos el resultado
	return xmlok(env);
}

XmlHandlerCmd jsr309CmdList[] =
{
	{"EventQueueCreate",			EventQueueCreate},
	{"EventQueueDelete",			EventQueueDelete},
	{"MediaSessionCreate",			MediaSessionCreate},
	{"MediaSessionDelete",			MediaSessionDelete},
	{"PlayerCreate",			PlayerCreate},
	{"PlayerOpen",				PlayerOpen},
	{"PlayerPlay",				PlayerPlay},
	{"PlayerSeek",				PlayerSeek},
	{"PlayerStop",				PlayerStop},
	{"PlayerClose",				PlayerClose},
	{"PlayerDelete",			PlayerDelete},
	{"RecorderCreate",			RecorderCreate},
	{"RecorderRecord",			RecorderRecord},
	{"RecorderStop",			RecorderStop},
	{"RecorderDelete",			RecorderDelete},
	{"RecorderAttachToEndpoint",		RecorderAttachToEndpoint},
	{"RecorderAttachToAudioMixerPort",	RecorderAttachToAudioMixerPort},
	{"RecorderAttachToVideoMixerPort",	RecorderAttachToVideoMixerPort},
	{"RecorderDettach",			RecorderDettach},
	{"EndpointCreate",			EndpointCreate},
	{"EndpointDelete",			EndpointDelete},
	{"EndpointStartSending",		EndpointStartSending},
	{"EndpointStopSending",			EndpointStopSending},
	{"EndpointStartReceiving",		EndpointStartReceiving},
	{"EndpointStopReceiving",		EndpointStopReceiving},
	{"EndpointRequestUpdate",		EndpointRequestUpdate},
	{"EndpointAttachToPlayer",		EndpointAttachToPlayer},
	{"EndpointAttachToEndpoint",		EndpointAttachToEndpoint},
	{"EndpointAttachToAudioMixerPort",	EndpointAttachToAudioMixerPort},
	{"EndpointAttachToVideoMixerPort",	EndpointAttachToVideoMixerPort},
	{"EndpointDettach",			EndpointDettach},
	{"AudioMixerCreate",			AudioMixerCreate},
	{"AudioMixerDelete",			AudioMixerDelete},
	{"AudioMixerPortCreate",		AudioMixerPortCreate},
	{"AudioMixerPortSetCodec",		AudioMixerPortSetCodec},
	{"AudioMixerPortDelete",		AudioMixerPortDelete},
	{"AudioMixerPortAttachToEndpoint",	AudioMixerPortAttachToEndpoint},
	{"AudioMixerPortAttachToPlayer",	AudioMixerPortAttachToPlayer},
	{"AudioMixerPortDettach",		AudioMixerPortDettach},
	{"VideoMixerCreate",			VideoMixerCreate},
	{"VideoMixerDelete",			VideoMixerDelete},
	{"VideoMixerPortCreate",		VideoMixerPortCreate},
	{"VideoMixerPortDelete",		VideoMixerPortDelete},
	{"VideoMixerPortSetCodec",		VideoMixerPortSetCodec},
	{"VideoMixerPortAttachToEndpoint",	VideoMixerPortAttachToEndpoint},
	{"VideoMixerPortAttachToPlayer",	VideoMixerPortAttachToPlayer},
	{"VideoMixerPortDettach",		VideoMixerPortDettach},
	{"VideoMixerMosaicCreate",		VideoMixerMosaicCreate},
	{"VideoMixerMosaicDelete",		VideoMixerMosaicDelete},
	{"VideoMixerMosaicSetSlot",		VideoMixerMosaicSetSlot},
	{"VideoMixerMosaicSetCompositionType",	VideoMixerMosaicSetCompositionType},
	{"VideoMixerMosaicSetOverlayPNG",	VideoMixerMosaicSetOverlayPNG},
	{"VideoMixerMosaicResetOverlay",	VideoMixerMosaicResetOverlay},
	{"VideoMixerMosaicAddPort",		VideoMixerMosaicAddPort},
	{"VideoMixerMosaicRemovePort",		VideoMixerMosaicRemovePort},
	{NULL,NULL}
};

