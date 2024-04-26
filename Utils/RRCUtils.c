#include "RRCUtils.h"
#include <UL-CCCH-Message.h>
#include <DL-CCCH-Message.h>

void RRCConnectionRequest_coder(uint8_t **buffer, ssize_t *len) {
    UL_CCCH_Message_t* msg;
    UL_CCCH_MessageType_t* msgType;
    RRCConnectionRequest_t *rrc = NULL;

    //Выделяем память
    msg = (UL_CCCH_Message_t*)calloc(1, sizeof(UL_CCCH_Message_t));
    msgType = (UL_CCCH_MessageType_t*)calloc(1, sizeof(UL_CCCH_MessageType_t));
    rrc = (RRCConnectionRequest_t*)calloc(1, sizeof(RRCConnectionRequest_t));

    uint8_t *spare = (uint8_t*)calloc(1, 1);
    *spare = 0;

    //Заполнение RRCConnectionRequest_t

    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.spare.buf = spare;
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.spare.size = 1;
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.spare.bits_unused = 0;
    
    rrc->criticalExtensions.present = RRCConnectionRequest__criticalExtensions_PR_rrcConnectionRequest_r8;
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.establishmentCause = EstablishmentCause_mo_Signalling;

    uint8_t* randv = (uint8_t*)calloc(2, 1);
    randv[0] = 1;
    randv[1] = 2;

    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.ue_Identity.present = InitialUE_Identity_PR_randomValue; 
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.ue_Identity.choice.randomValue.buf = randv;
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.ue_Identity.choice.randomValue.size = 2;
    rrc->criticalExtensions.choice.rrcConnectionRequest_r8.ue_Identity.choice.randomValue.bits_unused = 0;

    //Заполнение UL_CCCH_MessageType_t
    msgType->present = UL_CCCH_MessageType_PR_c1;
    msgType->choice.c1.present = UL_CCCH_MessageType__c1_PR_rrcConnectionRequest;
    msgType->choice.c1.choice.rrcConnectionRequest = *rrc;
    
    //Заполнение UL_CCCH_Message_t
    msg->message = *msgType;

    asn_encode_to_new_buffer_result_t res = {NULL, {0, NULL, NULL}};
    res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_UL_CCCH_Message, msg);
    printf("encoded: %ld\n", res.result.encoded);
    *buffer = res.buffer;
    *len = res.result.encoded;

    if (*buffer == NULL) {
        fprintf(stderr, "Error enconing rrc pdu\n");
        exit(1);
    } else {
        fprintf(stderr, "Encoded pdu\n");
        xer_fprint(stdout, &asn_DEF_UL_CCCH_Message, msg);
    }
}

void RRCConnectionSetup_coder(uint8_t **buffer, ssize_t *len) {
    DL_CCCH_Message_t* msg;
    DL_CCCH_MessageType_t* msgType;
    RRCConnectionSetup_t* rrc;

    msg = (DL_CCCH_Message_t*)calloc(1, sizeof(DL_CCCH_Message_t));
    msgType = (DL_CCCH_MessageType_t*)calloc(1, sizeof(DL_CCCH_MessageType_t));
    rrc = (RRCConnectionSetup_t*)calloc(1, sizeof(RRCConnectionSetup_t));

    //заполнение полей RRCConnectionSetup_t
    rrc->criticalExtensions.present = RRCConnectionSetup__criticalExtensions_PR_c1;
    rrc->criticalExtensions.choice.c1.present = RRCConnectionSetup__criticalExtensions__c1_PR_rrcConnectionSetup_r8;
    rrc->criticalExtensions.choice.c1.choice.rrcConnectionSetup_r8.radioResourceConfigDedicated.mac_MainConfig = (MAC_MainConfig_t*)calloc(1, sizeof(MAC_MainConfig_t));
    rrc->criticalExtensions.choice.c1.choice.rrcConnectionSetup_r8.radioResourceConfigDedicated.mac_MainConfig -> present = RadioResourceConfigDedicated__mac_MainConfig_PR_defaultValue;
    rrc->criticalExtensions.choice.c1.choice.rrcConnectionSetup_r8.radioResourceConfigDedicated.mac_MainConfig -> choice.defaultValue = 1;

    //заполнение DL_CCCH_MessageType_t
    msgType->present = DL_CCCH_MessageType_PR_c1;
    msgType->choice.c1.present = DL_CCCH_MessageType__c1_PR_rrcConnectionSetup;
    msgType->choice.c1.choice.rrcConnectionSetup = *rrc;

    //заполнение DL_CCCH_Message_t
    msg->message = *msgType;

    asn_encode_to_new_buffer_result_t res = {NULL, {0, NULL, NULL}};
    res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_DL_CCCH_Message, msg);
    printf("encoded: %ld\n", res.result.encoded);
    *buffer = res.buffer;
    *len = res.result.encoded;

    if (*buffer == NULL) {
        fprintf(stderr, "Error enconing rrc pdu\n");
        exit(1);
    } else {
        fprintf(stderr, "Encoded pdu\n");
        xer_fprint(stdout, &asn_DEF_DL_CCCH_Message, msg);
    }
}

void RRCConnectionRequest_decoder(uint8_t **buffer, ssize_t *len) {
    UL_CCCH_Message_t* msg = 0;
    asn_dec_rval_t rval; //decoder return value
    printf("Lenght:%ld\n", *len);
    rval = asn_decode(NULL, ATS_CANONICAL_XER, &asn_DEF_UL_CCCH_Message, (void**)&msg, *buffer, *len);
    if (rval.code == RC_FAIL) {
        printf("Fail to decode msg\n");
        exit(1);
    }
    xer_fprint(stdout, &asn_DEF_UL_CCCH_Message, msg);
}